CREATE OR REPLACE FUNCTION public.get_project_overview(p_pid INT)
 RETURNS TABLE (
    total_product_backlogs INT,
    finished_product_backlogs INT,
    total_story_points INT,
    finished_story_points INT,
    total_user_stories INT,
    finished_user_stories INT,
    failed_user_stories INT
 )
AS $$
    SELECT COUNT(DISTINCT pb.id) as total, 
        SUM(CASE WHEN pb.status = 'finished'::progress_type THEN 1 ELSE 0 END),
        SUM(pb.story_point), 
        SUM(CASE WHEN pb.status = 'finished'::progress_type THEN pb.story_point ELSE 0 END) ,
        COUNT(sb.id), 
        SUM(CASE WHEN sb.status = 'finished'::progress_type THEN 1 ELSE 0 END), 
        SUM(CASE WHEN sb.status = 'failed'::progress_type THEN 1 ELSE 0 END)
    FROM ProductBacklog pb
    LEFT JOIN SprintBacklog sb ON pb.id = sb.backlog_item_id
    WHERE pb.project_id = 2
    GROUP BY pb.project_id;
$$ LANGUAGE sql
;
