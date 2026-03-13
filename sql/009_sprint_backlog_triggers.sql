CREATE OR REPLACE FUNCTION handle_completeness_update()
RETURNS TRIGGER AS
$$
BEGIN
    IF OLD.status IN ('finished'::progress_type, 'failed'::progress_type) THEN
        RAISE EXCEPTION 'Status is % and cannot be changed further', OLD.status::TEXT;
    END IF;
    IF OLD.status IS DISTINCT FROM NEW.status THEN
        IF NEW.status = 'finished'::progress_type THEN
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