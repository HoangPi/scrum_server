CREATE OR REPLACE FUNCTION public.handle_completeness_update()
 RETURNS trigger
 LANGUAGE plpgsql
AS $function$
BEGIN
    IF OLD.task_owner IS DISTINCT FROM NEW.task_owner AND NEW.task_owner IS NULL THEN
        OLD.task_owner := NULL;
        RETURN OLD;
    END IF;
    IF OLD.status IN ('finished'::progress_type, 'failed'::progress_type) THEN
        RAISE EXCEPTION 'Status is % and cannot be changed further', OLD.status::TEXT;
    END IF;
    IF OLD.status IS DISTINCT FROM NEW.status THEN
        IF NEW.status = 'finished'::progress_type AND OLD.status <> 'in_review'::progress_type THEN
            RAISE EXCEPTION 'Cannot finish the backlog without reviewing';
        END IF;
        IF NEW.status = 'finished'::progress_type THEN
            IF NEW.task_owner IS NULL THEN
                RAISE EXCEPTION 'Cannot finish the task while there is no task owner';
            END IF;
            UPDATE ProductBacklog SET status = 'finished'::progress_type
            WHERE ProductBacklog.id = NEW.backlog_item_id;
        END IF;
		IF NEW.status = 'failed'::progress_type THEN
            UPDATE ProductBacklog SET status = 'created'::progress_type
            WHERE ProductBacklog.id = NEW.backlog_item_id;
		END IF;
    END IF;
    NEW.updated_at := CURRENT_DATE;
    RETURN NEW;
END
$function$
;

CREATE TRIGGER trg_check_status_before_update
BEFORE UPDATE ON SprintBacklog
FOR EACH ROW
EXECUTE FUNCTION handle_completeness_update();

CREATE OR REPLACE FUNCTION reset_backlog_status()
RETURNS TRIGGER AS
$$
BEGIN
    UPDATE ProductBacklog SET status = 'created'::progress_type
    WHERE ProductBacklog.id = OLD.backlog_item_id;
    RETURN NULL;
END
$$ language plpgsql;

CREATE TRIGGER trg_reset_back_log_status
AFTER DELETE ON SprintBacklog
FOR EACH ROW
EXECUTE FUNCTION reset_backlog_status();

CREATE OR REPLACE FUNCTION public.update_sprint_backlog(p_backlog_id integer, p_task_owner integer DEFAULT NULL::integer, p_status text DEFAULT NULL::text, p_notes text DEFAULT NULL::text)
 RETURNS void
 LANGUAGE plpgsql
AS $function$
DECLARE
    l_owner INT;
    l_status progress_type;
    l_notes TEXT;
    l_OLD SprintBacklog;
BEGIN
    SELECT * INTO l_OLD FROM SprintBacklog WHERE id = p_backlog_id;
    l_owner := p_task_owner;
    l_status := COALESCE(p_status, l_OLD.status::TEXT)::progress_type;
    l_notes := COALESCE(p_notes, l_OLD.notes);
    UPDATE SprintBacklog SET task_owner = l_owner,
        notes = l_notes, status = l_status
    WHERE SprintBacklog.id = l_OLD.id;
END 
$function$;

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

CREATE OR REPLACE FUNCTION auto_add_point()
RETURNS trigger
AS $$
BEGIN
    UPDATE SprintBacklog SET actual_story_point = actual_story_point + NEW.story_point WHERE id = NEW.sprint_backlog_id;
	RETURN NULL;
END $$ language plpgsql;

CREATE TRIGGER trg_auto_handle_story_points_on_create
AFTER INSERT ON Task
FOR EACH ROW
WHEN (NEW.finished)
EXECUTE FUNCTION auto_add_point();

CREATE OR REPLACE FUNCTION auto_remove_point()
RETURNS trigger
AS $$
BEGIN
    UPDATE SprintBacklog SET actual_story_point = actual_story_point - OLD.story_point WHERE id = OLD.sprint_backlog_id;
	RETURN NULL;
END $$ language plpgsql;

CREATE TRIGGER trg_auto_handle_story_points_on_remove
AFTER DELETE ON Task
FOR EACH ROW
WHEN (OLD.finished)
EXECUTE FUNCTION auto_remove_point();

create or replace function get_task_by_sprint_backlog_id(p_id int4)
returns table (
	id int,
	user_id int,
	sprint_backlog_id int,
	story_point int,
	finished boolean,
	name text,
	owner_name text,
	owner_email text
) as $$
begin
return query
SELECT t.id, t.user_id, t.sprint_backlog_id, t.story_point, t.finished, t.name, u.name AS owner_name, u.email AS owner_email 
            FROM Task t 
            LEFT JOIN AppUser u ON u.id = t.user_id 
            WHERE t.sprint_backlog_id = p_id;
end;
$$ language plpgsql;