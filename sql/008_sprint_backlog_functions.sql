CREATE OR REPLACE FUNCTION create_sprint_backlog(
    p_sprint_id INT, 
    p_product_backlog_id INT, 
    p_task_owner INT DEFAULT NULL,
    p_note TEXT DEFAULT '')
RETURNS VOID AS $$
DECLARE 
    l_task_owner INT;
    l_status progress_type;
    l_estimated_story_point INT;
    l_backlog_id INT;
BEGIN
    SELECT status INTO l_status FROM Sprint WHERE id = p_sprint_id;
    IF l_status = 'finished'::progress_type THEN
        RAISE EXCEPTION 'Cannot create sprint backlog for finished Sprint';
    END IF;
    SELECT story_point, id INTO l_estimated_story_point, l_backlog_id FROM ProductBacklog WHERE id = p_product_backlog_id;
    IF EXISTS (
        SELECT sb.id FROM SprintBacklog sb
        WHERE sb.backlog_item_id = p_product_backlog_id AND sb.status <> 'failed'::progress_type
        LIMIT 1
    ) THEN
        RAISE EXCEPTION 'Product backlog is completed or registered in a different sprint';
    END IF;

    IF EXISTS (
        SELECT s.id FROM Sprint s WHERE s.id = p_sprint_id AND s.status = 'on_going'::progress_type
    ) THEN
        l_task_owner := p_task_owner;
    ELSE
        l_task_owner := NULL;
    END IF;

    IF l_task_owner IS NULL THEN
        l_status = 'created'::progress_type;
    ELSE
        l_status = 'on_going'::progress_type;
    END IF;

    INSERT INTO SprintBacklog 
        (sprint_id, backlog_item_id, task_owner, status, notes, estimated_story_point)
    VALUES (p_sprint_id, p_product_backlog_id, l_task_owner, l_status, p_note, l_estimated_story_point);
    UPDATE ProductBacklog SET status = 'on_going'::progress_type WHERE id = p_product_backlog_id;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_product_backlog(p_project_id INT, p_offset INT DEFAULT 0, p_include_finished BOOLEAN DEFAULT NULL)
RETURNS TABLE (
    id INT,
    project_id INT,
    name TEXT,
    acceptance_criteria TEXT,
    priority INT,
    status TEXT,
    story_point INT
) AS $$
DECLARE
    l_status progress_type;
BEGIN
    IF p_include_finished IS NULL THEN
        RETURN QUERY
        SELECT pb.id, pb.project_id, pb.name, pb.acceptance_criteria,
            pb.priority, pb.status::TEXT, pb.story_point
        FROM ProductBacklog pb 
        WHERE pb.project_id = p_project_id
        LIMIT 20 OFFSET (20 * p_offset);
    ELSE
        IF p_include_finished THEN
            l_status = 'finished'::progress_type;
        ELSE
            l_status = 'created'::progress_type;
        END IF;

        RETURN QUERY
        SELECT pb.id, pb.project_id, pb.name, pb.acceptance_criteria,
            pb.priority, pb.status::TEXT, pb.story_point
        FROM ProductBacklog pb 
        WHERE pb.project_id = p_project_id AND pb.status = l_status
        LIMIT 20 OFFSET (20 * p_offset);
    END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_sprint_backlog_by_sprint_id(p_sprint_id INT)
RETURNS TABLE (
    id INT,
    sprint_id INT,
    backlog_item_id INT,
    backlog_name TEXT,
    priority INT,
    task_owner INT,
    owner_name TEXT,
    email TEXT,
    status TEXT,
    notes TEXT,
    acceptance_criteria TEXT,
    estimated_story_point INT,
    actual_story_point INT
) AS $$
BEGIN
    RETURN QUERY
    SELECT sb.id, sb.sprint_id, pb.id, pb.name, pb.priority,
        m.id, m.name::TEXT, m.email::TEXT, sb.status::TEXT, sb.notes, 
        pb.acceptance_criteria, sb.estimated_story_point, sb.actual_story_point
    FROM SprintBacklog sb
    INNER JOIN ProductBacklog pb ON pb.id = sb.backlog_item_id
    LEFT JOIN AppUser m ON m.id = sb.task_owner
    WHERE sb.sprint_id = p_sprint_id;
END;
$$ LANGUAGE plpgsql;