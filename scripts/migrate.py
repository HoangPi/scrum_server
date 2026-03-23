import os
from dotenv import load_dotenv
import psycopg2
import users
import sprint

# Load environment variables from .env
load_dotenv('../.env')

# Read variables
host = os.getenv("DB_HOST")
database = os.getenv("DB_NAME")
user = os.getenv("DB_USER")
password = os.getenv("DB_PASSWORD")
port = os.getenv("DB_PORT")
print(host, database, user, password, port)

# Connect to PostgreSQL
conn = psycopg2.connect(
    host=host,
    database=database,
    user=user,
    password=password,
    port=port
)

cursor = conn.cursor()
cursor.execute("SELECT version();")
print(cursor.fetchone())

users.process_user_credentials('mock_user.json', 'records/users.json', conn)
users.setup_project_and_members(conn, 'SCRUM management', 'Build a project to manage a project following SCRUM framework')
sprint.setup_project_sprints(conn, 'mock_sprint.json')
sprint.setup_product_backlog(conn, 'mock_backlog.json')