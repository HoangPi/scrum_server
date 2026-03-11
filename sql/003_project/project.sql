CREATE TABLE Project (
    id SERIAL PRIMARY KEY,
    name TEXT NOT NULL,
    description TEXT DEFAULT NULL,
    project_owner INT NOT NULL REFERENCES AppUser(id) ON DELETE CASCADE,
    -- week
    sprint_length INT NOT NULL DEFAULT 2,
    sprint_start_day INT NOT NULL DEFAULT 0 CHECK (sprint_start_day BETWEEN 0 AND 6),
    work_day_hour INT NOT NULL DEFAULT 6 CHECK (work_day_hour BETWEEN 1 AND 24),
    -- 0b1100000
    default_day_off INT NOT NULL DEFAULT 96 CHECK (default_day_off >= 0 AND default_day_off <= 127),
    status TEXT NOT NULL DEFAULT 'active',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE Member (
    user_id INT NOT NULL REFERENCES AppUser(id) ON DELETE CASCADE,
    project_id INT NOT NULL REFERENCES Project(id) ON DELETE CASCADE,
    role TEXT NOT NULL DEFAULT 'EM' CHECK (role IN ('EM', 'PO', 'SM')),

    PRIMARY KEY (user_id, project_id)
);

CREATE OR REPLACE FUNCTION create_po_member()
RETURNS trigger AS $$
BEGIN
    INSERT INTO Member (user_id, project_id, role) VALUES (NEW.project_owner, NEW.id, 'PO');
    RETURN NULL;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_create_po_member_on_insert
AFTER INSERT ON Project
FOR EACH ROW
EXECUTE FUNCTION create_po_member();

CREATE OR REPLACE FUNCTION member_update_check()
RETURNS trigger AS $$
BEGIN
    IF OLD.role = 'PO' OR NEW.role = 'PO' THEN
        RAISE EXCEPTION 'Changing memeber involving Project Owner is not allowed';
    END IF;
    IF OLD.user_id IS DISTINCT FROM NEW.user_id OR OLD.project_id IS DISTINCT FROM NEW.project_id THEN
        RAISE EXCEPTION 'Update on user_id or project_id is not allowed';
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_prevent_jumping_between_projects
BEFORE UPDATE ON Project
FOR EACH ROW
EXECUTE FUNCTION member_update_check();