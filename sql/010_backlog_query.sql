CREATE OR REPLACE FUNCTION public.get_product_backlog_with_query(
	p_project_id integer, 
	p_offset integer DEFAULT 0, 
	p_name_filter text default '',
	p_include_finished boolean DEFAULT NULL::boolean,
	p_asc_story_point boolean default null::BOOLEAN,
	p_asc_priority boolean default null::boolean)
 RETURNS TABLE(id integer, project_id integer, name text, acceptance_criteria text, priority integer, status text, story_point integer)
 LANGUAGE plpgsql
AS $function$
DECLARE
    l_status text;
	l_sql TEXT;
	l_include_text TEXT := '';
	l_order_text TEXT := '';
BEGIN
	l_sql := 	'SELECT pb.id, pb.project_id, pb.name, pb.acceptance_criteria, '
            'pb.priority, pb.status::TEXT, pb.story_point '
        	'FROM ProductBacklog pb '
        	'WHERE pb.project_id = ' || p_project_id || ' AND pb.name LIKE ''%' || p_name_filter || '%''';
	IF p_include_finished IS NOT NULL THEN 
		IF p_include_finished = false THEN
			l_include_text := ' AND pb.status <> ''finished''::progress_type ';
		END IF;
	ELSE
		l_include_text := ' AND pb.status = ''created''::progress_type ';
	END IF;
	l_sql := l_sql || l_include_text;

	IF p_asc_story_point IS NOT NULL THEN
		IF p_asc_story_point THEN
			l_order_text := 'ORDER BY pb.story_point ASC';
		ELSE
			l_order_text := 'ORDER BY pb.story_point DESC';
		END IF;
	END IF;
	IF p_asc_priority IS NOT NULL THEN
		IF l_order_text <> '' THEN
			l_order_text := l_order_text || ', pb.priority ';
		ELSE
			l_order_text := 'ORDER BY pb.priority ';
		END IF;
		IF p_asc_priority THEN
			l_order_text := l_order_text || 'ASC';
		ELSE
			l_order_text := l_order_text || 'DESC';
		END IF;
	END IF;
	l_sql := l_sql || l_order_text;

	l_sql := l_sql || ' LIMIT 20 OFFSET (' || p_offset || ' * 20);';
	RETURN QUERY EXECUTE l_sql;
END;
$function$