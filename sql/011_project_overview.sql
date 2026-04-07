CREATE OR REPLACE FUNCTION public.get_project_overview(p_pid integer)
 RETURNS TABLE(total_product_backlogs integer, finished_product_backlogs integer, total_story_points integer, finished_story_points integer, total_user_stories integer, finished_user_stories integer, failed_user_stories integer)
 LANGUAGE sql
AS $function$
    SELECT 
	    COUNT(DISTINCT pb.id) AS total,
	    SUM(CASE WHEN pb.status = 'finished'::progress_type THEN 1 ELSE 0 END) AS finished_pb,
	    SUM(pb.story_point) AS total_story_points,
	    SUM(CASE WHEN pb.status = 'finished'::progress_type THEN pb.story_point ELSE 0 END) AS finished_story_points,
	    SUM(sb_stats.total_sb),
	    SUM(sb_stats.finished_sb),
	    SUM(sb_stats.failed_sb)
	FROM ProductBacklog pb
	LEFT JOIN (
	    SELECT backlog_item_id,
	           COUNT(DISTINCT id) AS total_sb,
	           SUM(CASE WHEN status = 'finished'::progress_type THEN 1 ELSE 0 END) AS finished_sb,
	           SUM(CASE WHEN status = 'failed'::progress_type THEN 1 ELSE 0 END) AS failed_sb
	    FROM SprintBacklog
	    GROUP BY backlog_item_id
	) sb_stats ON pb.id = sb_stats.backlog_item_id
	WHERE pb.project_id = p_pid
	GROUP BY pb.project_id;
$function$
;