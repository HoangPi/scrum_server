CREATE OR REPLACE FUNCTION create_sprint_backlog(
    p_sprint_id INT, 
    p_product_backlog_id INT, 
    p_task_owner INT DEFAULT NULL,
    p_note TEXT DEFAULT '',
    p_estimated_story_point INT DEFAULT 0)
RETURNS VOID AS $$
DECLARE 
    l_task_owner INT;
    l_status progress_type;
BEGIN
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
    VALUES (p_sprint_id, p_product_backlog_id, l_task_owner, l_status, p_note, p_estimated_story_point);
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION create_sprint_backlog(p_project_id INT, p_offset INT DEFAULT 0, p_include_finished BOOLEAN DEFAULT NULL)
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