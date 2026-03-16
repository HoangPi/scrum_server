ALTER TABLE AppUser
ADD COLUMN created_at TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP NOT NULL,
ADD COLUMN updated_at TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP NOT NULL,
ADD COLUMN name TEXT NOT NULL DEFAULT 'name',
ADD COLUMN deleted_at TIMESTAMPTZ DEFAULT NULL;
ALTER TABLE public.appuser ALTER COLUMN email TYPE text USING email::text;

CREATE OR REPLACE FUNCTION get_user_by_id(p_id BIGINT)
RETURNS TABLE (
    id INT,
    username TEXT,
    name TEXT,
    email TEXT,
    role TEXT,
    created_at BIGINT,
    updated_at BIGINT
) AS $$
    SELECT u.id, u.username, u.name, u.email, u.role, EXTRACT(EPOCH FROM u.created_at)::BIGINT, EXTRACT(EPOCH FROM u.updated_at)::BIGINT
    FROM AppUser u
    WHERE u.id = p_id
    LIMIT 1;
$$ LANGUAGE sql;

CREATE OR REPLACE FUNCTION get_user_by_username(p_username TEXT)
RETURNS TABLE (
    id INT,
    username TEXT,
    name TEXT,
    password TEXT,
    email TEXT,
    role TEXT,
    created_at BIGINT,
    updated_at BIGINT
) AS $$
    SELECT u.id, u.username, u.name, u.password, u.email, u.role, EXTRACT(EPOCH FROM u.created_at)::BIGINT, EXTRACT(EPOCH FROM u.updated_at)::BIGINT
    FROM AppUser u
    WHERE u.username = p_username
    LIMIT 1;
$$ LANGUAGE sql;