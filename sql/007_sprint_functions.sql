CREATE OR REPLACE FUNCTION public.create_sprint(p_project_id integer, p_name text, p_goal text, p_start_date text)
 RETURNS sprint
 LANGUAGE plpgsql
AS $function$
DECLARE
    l_sprint_length INT;
    l_status progress_type;
    l_end_date DATE;
    l_new_sprint Sprint;
	l_start_date DATE;
BEGIN
    SELECT sprint_length INTO l_sprint_length 
    FROM Project 
    WHERE id = p_project_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Project with ID % not found', p_project_id;
    END IF;
	IF p_start_date = '' THEN
		SELECT MAX(end_date) INTO l_start_date FROM Sprint WHERE project_id = p_project_id GROUP BY project_id;
		l_start_date := l_start_date + INTERVAL '1 day';
	ELSE	
		l_start_date := p_start_date::DATE;
	END IF;
    l_end_date := l_start_date::DATE + (l_sprint_length * 7 * INTERVAL '1 day') - (INTERVAL '1 day');
    IF l_start_date < CURRENT_DATE AND CURRENT_DATE < l_end_date THEN
        l_status = 'on_going'::progress_type;
    ELSE
        l_status = 'created'::progress_type;
    END IF;

    INSERT INTO Sprint (project_id, name, goal, start_date, end_date, status)
    VALUES (p_project_id, p_name, p_goal, 
        l_start_date, l_end_date, l_status)
    RETURNING * INTO l_new_sprint;
    RETURN l_new_sprint;
END;
$function$
;

CREATE OR REPLACE FUNCTION get_sprints_in_between_dates(p_project_id INT, p_start TEXT, p_end TEXT)
RETURNS TABLE (
    id INT,
    project_id INT,
    name TEXT,
    goal TEXT,
    start_date TEXT,
    end_date TEXT,
    status TEXT
) AS $$
BEGIN
    RETURN QUERY
    SELECT s.id, s.project_id, s.name, s.goal, s.start_date::TEXT, s.end_date::TEXT, s.status::TEXT
    FROM Sprint s
    WHERE s.project_id = p_project_id 
        AND s.start_date <= p_end::DATE 
        AND s.end_date >= p_start::DATE
    ORDER BY start_date
    LIMIT 20;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION update_sprint (p_name TEXT, p_goal TEXT, p_status TEXT)
RETURNS Void AS $$
BEGIN
    UPDATE Sprint SET name = p_name, goal = p_goal, status = p_status;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION public.prevent_finshed_sprint_update()
 RETURNS trigger
 LANGUAGE plpgsql
AS $function$
BEGIN
    -- allowed expired end date update for mock data creation
    IF OLD.status = 'finished'::progress_type THEN
        RAISE EXCEPTION 'Sprint has ended and cannot be modify further';
    END IF;
	IF NEW.status = 'finished'::progress_type THEN
		UPDATE public.SprintBacklog SET status = 'failed'::progress_type
			WHERE sprint_id = NEW.id AND status NOT IN ('finished'::progress_type, 'failed'::progress_type);
	END IF;
    RETURN NEW;
END;
$function$;

CREATE TRIGGER trg_check_sprint_status
BEFORE UPDATE ON Sprint
FOR EACH ROW
EXECUTE FUNCTION prevent_finshed_sprint_update();

CREATE EXTENSION IF NOT EXISTS btree_gist;

ALTER TABLE Sprint
ADD CONSTRAINT exclude_overlapping_sprints
EXCLUDE USING gist (
  project_id WITH =,
  daterange(start_date, end_date, '[]') WITH &&
);