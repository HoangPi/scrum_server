import json
import secrets
import string
import bcrypt

def process_user_credentials(input_filepath: str, output_filepath: str, db_connection):
    """
    Reads user data, generates unique passwords (excluding quotation marks), 
    saves plaintext credentials, hashes the passwords, and pushes users 
    to the database using the provided connection.
    """
    with open(input_filepath, 'r') as infile:
        users = json.load(infile)

    credentials_to_save = []
    generated_passwords = set()
    
    safe_punctuation = string.punctuation.replace('"', '')
    alphabet = string.ascii_letters + string.digits + safe_punctuation

    # Initialize the database cursor
    cursor = db_connection.cursor()
    
    # Prepare the SQL insert statement
    insert_query = """
        INSERT INTO AppUser (id, username, email, name, password)
        VALUES (%s, %s, %s, %s, %s)
    """
    i = 1
    for user in users:
        while True:
            password = ''.join(secrets.choice(alphabet) for _ in range(16))
            
            if password not in generated_passwords:
                generated_passwords.add(password)
                break

        credentials_to_save.append({
            "username": user["username"],
            "password": password
        })

        salt = bcrypt.gensalt()
        hashed_password = bcrypt.hashpw(password.encode('utf-8'), salt)
        hashed_password_str = hashed_password.decode('utf-8')
        
        # Push to the database, mapping the hashed password string to the password column
        cursor.execute(
            insert_query,
            (
                i, 
                user["username"], 
                user["email"], 
                user["name"], 
                hashed_password_str
            )
        )
        i += 1

    # Commit the transaction to apply all inserts
    db_connection.commit()
    cursor.close()

    with open(output_filepath, 'w') as outfile:
        json.dump(credentials_to_save, outfile, indent=4)
        
    return users

def setup_project_and_members(db_connection, project_name, project_description):
    """
    Inserts a specific project into the database and assigns users 2, 3, and 4 
    to it with roles determined by their user ID.
    """
    cursor = db_connection.cursor()
    
    # 1. Insert the project details
    insert_project_query = """
        INSERT INTO project 
        (id, name, description, project_owner, sprint_length, work_day_hour, default_day_off, status)
        VALUES (%s, %s, %s, %s, %s, %s, %s, %s)
    """
    project_values = (
        1, 
        project_name, 
        project_description, 
        1,  # project_owner
        2,  # sprint_length
        6,  # work_day_hour
        96, # default_day_off
        'on_going' # status
    )
    
    cursor.execute(insert_project_query, project_values)
    
    # 2. Insert the members into the project
    insert_member_query = """
        INSERT INTO member (user_id, project_id, role)
        VALUES (%s, %s, %s)
    """
    
    # Loop through user IDs 2, 3, and 4
    for user_id in range(2, 5):
        # Apply the role logic inline: 'SM' for user 4, 'EM' for the rest
        role = 'SM' if user_id == 4 else 'EM'
        
        cursor.execute(insert_member_query, (user_id, 1, role))

    # Commit the transaction to apply both the project and member inserts
    db_connection.commit()
    cursor.close()