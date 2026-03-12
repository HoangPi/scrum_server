CREATE OR REPLACE FUNCTION create_sprint (
    p_project_id INT,
    p_name TEXT,
    p_goal TEXT,
    p_start_date DATE)
RETURNS Sprint AS $$
DECLARE
    l_sprint_length INT;
    l_status progress_type;
    l_end_date DATE;
    l_new_sprint Sprint;
BEGIN
    SELECT sprint_length INTO l_sprint_length 
    FROM Project 
    WHERE id = p_project_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Project with ID % not found', p_project_id;
    END IF;

    l_end_date := p_start_date + (l_sprint_length * 7 * INTERVAL '1 day');
    IF p_start_date < CURRENT_DATE AND CURRENT_DATE < l_end_date THEN
        l_status = 'on_going'::progress_type;
    ELSE
        l_status = 'created'::progress_type;
    END IF;

    INSERT INTO Sprint (project_id, name, goal, start_date, end_date, status)
    VALUES (p_project_id, p_name, p_goal, 
        p_start_date, l_end_date, l_status)
    RETURNING * INTO l_new_sprint;
    RETURN l_new_sprint;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION update_sprint (p_name TEXT, p_goal TEXT, p_status TEXT)
RETURNS Void AS $$
BEGIN
    UPDATE Sprint SET name = p_name, goal = p_goal, status = p_status;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION prevent_finshed_sprint_update()
RETURNS trigger AS $$
BEGIN
    -- allowed expired end date update for mock data creation
    IF OLD.status = 'finished'::progress_type THEN
        RAISE EXCEPTION 'Sprint has ended and cannot be modify futher';
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_check_sprint_status
BEFORE UPDATE ON Sprint
FOR EACH ROW
EXECUTE FUNCTION prevent_finshed_sprint_update();