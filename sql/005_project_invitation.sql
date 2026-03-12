CREATE TYPE invitation_status AS ENUM ('created', 'accepted', 'declined', 'expired');

CREATE TABLE invite(
    id SERIAL PRIMARY KEY,
    invitee INT NOT NULL REFERENCES AppUser(id) ON DELETE CASCADE,
    project_id INT NOT NULL REFERENCES Project(id) ON DELETE CASCADE,
    expired_at TIMESTAMP NOT NULL DEFAULT (CURRENT_TIMESTAMP + INTERVAL '7 days'),
    status invitation_status NOT NULL DEFAULT 'created'::invitation_status
);

CREATE OR REPLACE FUNCTION invite_update_check()
RETURNS trigger AS $$
BEGIN
    IF OLD.status <> 'created'::invitation_status THEN
        RAISE EXCEPTION 'Invitation has been % and cannot be modified futher', OLD.status;
    END IF;
    IF OLD.expired_at <= CURRENT_TIMESTAMP THEN
        RAISE EXCEPTION 'Invitation has expired';
        -- Raise but not update because there will be a different task for it
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION add_member_on_accepted()
RETURNS trigger AS $$
BEGIN
    IF NEW.status = 'accepted'::invitation_status THEN
        INSERT INTO Member (user_id, project_id, role) VALUES (NEW.invitee, NEW.project_id, 'EM');
    END IF;
    RETURN NULL;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION create_notification_for_invite()
RETURNS trigger AS $$
DECLARE
    v_PO_name TEXT;
    v_project_name TEXT;
    v_message TEXT;
    v_path TEXT;
BEGIN
    SELECT u.name, p.name
    INTO v_PO_name, v_project_name
    FROM Project p
    JOIN AppUser u ON u.id = p.project_owner
    WHERE p.id = NEW.project_id;
    v_message := format('%s has invited you to project %s', v_PO_name, v_project_name);
    v_path := format('invite/project?id=%s', NEW.id);
    INSERT INTO notification (priority, message, path) VALUES (0, v_message, v_path);
    RETURN NULL;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION delete_duplicate_invite()
RETURNS trigger AS $$
BEGIN
    DELETE FROM Invite
    WHERE project_id = NEW.project_id AND invitee = NEW.invitee AND status = 'created'::invitation_status;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_auto_create_notification
AFTER INSERT ON invite
FOR EACH ROW
EXECUTE FUNCTION create_notification_for_invite();

CREATE TRIGGER trg_auto_delete_duplicate
BEFORE INSERT ON invite
FOR EACH ROW
EXECUTE FUNCTION delete_duplicate_invite();

CREATE TRIGGER trg_check_invitation_condition
BEFORE UPDATE ON invite
FOR EACH ROW
EXECUTE FUNCTION invite_update_check();

CREATE TRIGGER trg_auto_add_member
AFTER UPDATE ON invite
FOR EACH ROW
EXECUTE FUNCTION add_member_on_accepted();