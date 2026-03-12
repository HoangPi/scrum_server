CREATE TYPE progress_type AS ENUM ('created', 'on_going', 'finished', 'failed', 'in_review');

CREATE TABLE ProductBacklog (
    id SERIAL PRIMARY KEY,

    project_id INT NOT NULL REFERENCES Project(id) ON DELETE CASCADE,
    name TEXT NOT NULL,
    acceptance_criteria TEXT NOT NULL DEFAULT '',
    priority INT NOT NULL CHECK (priority >= 0) DEFAULT 0,
    status progress_type NOT NULL CHECK (status NOT IN ('on_going'::progress_type, 'failed'::progress_type, 'in_review'::progress_type)) DEFAULT 'created'::progress_type,
    story_point INT NOT NULL CHECK (story_point >= 0) DEFAULT 0,

    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE Sprint (
    id SERIAL PRIMARY KEY,

    project_id INT NOT NULL REFERENCES Project(id) ON DELETE CASCADE,
    name TEXT NOT NULL,
    goal TEXT NOT NULL,
    start_date DATE NOT NULL,
    end_date DATE NOT NULL,
    status progress_type NOT NULL CHECK (status NOT IN ('failed'::progress_type, 'in_review'::progress_type)) DEFAULT 'created'::progress_type,
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE SprintBacklog (
    id SERIAL PRIMARY KEY,

    sprint_id INT NOT NULL REFERENCES Sprint(id) ON DELETE CASCADE,
    backlog_item_id INT NOT NULL REFERENCES ProductBacklog(id) ON DELETE CASCADE,
    task_owner INT DEFAULT NULL REFERENCES AppUser(id) ON DELETE SET NULL,

    status progress_type NOT NULL DEFAULT 'created'::progress_type,
    notes TEXT DEFAULT '',
    estimated_story_point INT CHECK (estimated_story_point >= 0),
    actual_story_point INT CHECK (actual_story_point >= 0),
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    UNIQUE (sprint_id, backlog_item_id)
);

CREATE TABLE Task (
    id SERIAL PRIMARY KEY,

    user_id INT REFERENCES AppUser(id) ON DELETE SET NULL,
    sprint_backlog_id INT NOT NULL REFERENCES SprintBacklog(id) ON DELETE CASCADE,
    story_point INT NOT NULL CHECK (story_point >= 0) DEFAULT 0
);