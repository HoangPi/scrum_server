CREATE OR REPLACE FUNCTION handle_completeness_update()
RETURNS TRIGGER AS
$$
BEGIN
    IF OLD.status IN ('finished'::progress_type, 'failed'::progress_type) THEN
        RAISE EXCEPTION 'Status is % and cannot be changed further', OLD.status::TEXT;
    END IF;
    IF OLD.status IS DISTINCT FROM NEW.status THEN
        IF NEW.status = 'finished'::progress_type THEN
            IF NEW.task_owner IS NULL THEN
                RAISE EXCEPTION 'Cannot finish the task while there is no task owner';
            END IF;
            UPDATE ProductBacklog SET status = 'finished'::progress_type
            WHERE ProductBacklog.id = NEW.backlog_item_id;
        END IF;
    END IF;
    NEW.updated_at := CURRENT_DATE;
    RETURN NEW;
END
$$ language plpgsql;

CREATE TRIGGER trg_check_status_before_update
BEFORE UPDATE ON SprintBacklog
FOR EACH ROW
WHEN (NOT (OLD.task_owner IS NOT NULL AND NEW.task_owner IS NULL))
EXECUTE FUNCTION handle_completeness_update();

CREATE OR REPLACE FUNCTION reset_backlog_status()
RETURNS TRIGGER AS
$$
BEGIN
    IF OLD.status = 'finished'::progress_type THEN
        UPDATE ProductBacklog SET status = 'created'::progress_type
        WHERE ProductBacklog.id = OLD.backlog_item_id;
    END IF;
    RETURN NULL;
END
$$ language plpgsql;

CREATE TRIGGER trg_reset_back_log_status
AFTER DELETE ON SprintBacklog
FOR EACH ROW
WHEN (OLD.status = 'finished'::progress_type)
EXECUTE FUNCTION reset_backlog_status();

CREATE OR REPLACE FUNCTION update_sprint_backlog(
    p_backlog_id INT, 
    p_task_owner INT DEFAULT NULL,
    p_status TEXT DEFAULT NULL,
    p_notes TEXT DEFAULT NULL)
RETURNS VOID AS $$
DECLARE
    l_owner INT;
    l_status progress_type;
    l_notes TEXT;
    l_OLD SprintBacklog;
BEGIN
    SELECT * INTO l_OLD FROM SprintBacklog WHERE id = p_backlog_id;
    l_owner := COALESCE(p_task_owner, l_OLD.task_owner);
    l_status := COALESCE(p_status, l_OLD.status::TEXT)::progress_type;
    l_notes := COALESCE(p_notes, l_OLD.notes);
    UPDATE SprintBacklog SET task_owner = l_owner,
        notes = l_notes, status = l_status
    WHERE SprintBacklog.id = l_OLD.id;
END 
$$ language plpgsql;

ALTER TABLE Task ADD COLUMN finished BOOLEAN NOT NULL DEFAULT false;
ALTER TABLE Task ADD COLUMN name TEXT DEFAULT NULL;
CREATE OR REPLACE FUNCTION handle_story_point_update()
RETURNS trigger
AS $$
DECLARE
    old_active_points INT :=0;
    new_active_points INT :=0;
    point_difference INT :=0;
BEGIN
    IF OLD.finished THEN
        old_active_points := OLD.story_point;
    END IF;
    IF NEW.finished THEN
        new_active_points := NEW.story_point;
    END IF;
    point_difference := new_active_points - old_active_points;

    IF point_difference <> 0 THEN
        UPDATE SprintBacklog 
        SET actual_story_point = actual_story_point + point_difference
        WHERE id = NEW.sprint_backlog_id; 
    END IF;
    RETURN NEW;
END $$ language plpgsql;

CREATE TRIGGER trg_auto_handle_story_points
AFTER UPDATE ON Task
FOR EACH ROW
EXECUTE FUNCTION handle_story_point_update();