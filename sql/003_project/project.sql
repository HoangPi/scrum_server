CREATE TABLE Project (
    id SERIAL PRIMARY KEY,
    name TEXT NOT NULL,
    description TEXT DEFAULT NULL,
    project_owner INT NOT NULL REFERENCES AppUser(id) ON DELETE CASCADE,
    -- week
    sprint_length INT DEFAULT 2,
    sprint_start_day INT DEFAULT 0 CHECK (sprint_start_day BETWEEN 0 AND 6),
    work_day_hour INT DEFAULT 6 CHECK (work_day_hour BETWEEN 1 AND 24),
    -- 0b1100000
    default_day_off INT DEFAULT 96 CHECK (default_day_off >= 0 AND default_day_off <= 127),
    status TEXT DEFAULT 'active',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
)