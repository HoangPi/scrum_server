import json
from datetime import datetime, timedelta

def setup_project_sprints(db_connection, json_filepath):
    """
    Reads 15 sprints from a JSON file and creates them in the database
    by calling the create_sprint function with calculated rolling start dates.
    """
    with open(json_filepath, 'r') as infile:
        sprint_data = json.load(infile)

    cursor = db_connection.cursor()
    
    # Calculate initial date: current date - (11 * 14 days)
    current_date = datetime.now().date()
    current_start_date = current_date - timedelta(days=154)

    # Loop through exactly 15 sprints
    for sprint in sprint_data[:15]:
        # Convert the datetime object to the required 'yyyy-mm-dd' string format
        formatted_date = current_start_date.strftime('%Y-%m-%d')
        
        # Execute the database function with the precise parameters requested
        cursor.execute(
            "SELECT create_sprint(%s, %s, %s, %s);",
            (
                1,                      # p_project_id
                sprint["name"],         # p_name
                sprint["goal"],         # p_goal
                formatted_date          # p_start_date
            )
        )
        
        # Increment the start date by exactly 14 days for the next iteration
        current_start_date += timedelta(days=14)

    db_connection.commit()
    cursor.close()
    
def setup_product_backlog(db_connection, json_filepath):
    """
    Reads up to 20 product backlog items from a JSON file and inserts 
    them directly into the ProductBacklog table.
    """
    with open(json_filepath, 'r') as infile:
        backlog_data = json.load(infile)

    cursor = db_connection.cursor()
    
    insert_backlog_query = """
        INSERT INTO ProductBacklog 
        (project_id, name, acceptance_criteria, priority, story_point)
        VALUES (%s, %s, %s, %s, %s)
    """

    # Ensure we strictly process only the first 20 items
    for item in backlog_data[:20]:
        cursor.execute(
            insert_backlog_query,
            (
                item["project_id"],
                item["name"],
                item["acceptance_criteria"],
                item["priority"],
                item["story_point"]
            )
        )

    db_connection.commit()
    cursor.close()