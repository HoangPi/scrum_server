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