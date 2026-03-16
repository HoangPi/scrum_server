#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class IdDto : public oatpp::DTO
{
    DTO_INIT(IdDto, DTO)

    DTO_FIELD(Int32, id, "id");
};

class CreateSprintDto : public oatpp::DTO
{

    DTO_INIT(CreateSprintDto, DTO)

    DTO_FIELD(Int32, projectId, "project_id");
    DTO_FIELD(String, name, "name");
    DTO_FIELD(String, goal, "goal");
    DTO_FIELD(String, startDate, "start_date");
};

class SprintDto : public oatpp::DTO
{

    DTO_INIT(SprintDto, DTO)

    DTO_FIELD(Int32, id, "id");
    DTO_FIELD(Int32, projectId, "project_id");
    DTO_FIELD(String, name, "name");
    DTO_FIELD(String, goal, "goal");
    DTO_FIELD(String, startDate, "start_date");
    DTO_FIELD(String, endDate, "end_date");
    DTO_FIELD(String, status, "status");
};

class CreateSprintBacklogDto : public oatpp::DTO
{
    DTO_INIT(CreateSprintBacklogDto, DTO)

    DTO_FIELD(Int32, sprintId, "sprint_id");                  // maps to p_sprint_id
    DTO_FIELD(Int32, productBacklogId, "product_backlog_id"); // maps to p_product_backlog_id
    DTO_FIELD(Int32, taskOwner, "task_owner");                // maps to p_task_owner (optional)
    DTO_FIELD(String, note, "note");                          // maps to p_note
};

class UpdateSprintBacklogDto : public oatpp::DTO
{
    DTO_INIT(UpdateSprintBacklogDto, DTO)

    DTO_FIELD(Int32, id, "id");
    DTO_FIELD(Int32, taskOwner, "taskOwner") = nullptr;
    DTO_FIELD(String, note, "notes") = nullptr;
    DTO_FIELD(String, status, "status") = nullptr;
};

class SprintBacklogDetailDto : public oatpp::DTO
{
    DTO_INIT(SprintBacklogDetailDto, DTO)

    DTO_FIELD(Int32, id);
    DTO_FIELD(Int32, sprintId, "sprint_id");
    DTO_FIELD(Int32, backlogItemId, "backlog_item_id");
    DTO_FIELD(String, backlogName, "backlog_name");
    DTO_FIELD(Int32, priority);

    DTO_FIELD(Int32, taskOwner, "task_owner");
    DTO_FIELD(String, ownerName, "owner_name");
    DTO_FIELD(String, email);

    DTO_FIELD(String, status);
    DTO_FIELD(String, notes);
    DTO_FIELD(Int32, estimatedStoryPoint, "estimated_story_point");
    DTO_FIELD(Int32, actualStoryPoint, "actual_story_point");
};

class TaskDto : public oatpp::DTO
{
    DTO_INIT(TaskDto, DTO)

    DTO_FIELD(Int32, id, "id");
    DTO_FIELD(Int32, taskOwner, "user_id") = nullptr;
    DTO_FIELD(Int32, sprintBacklogId, "sprint_backlog_id") = nullptr;
    DTO_FIELD(Int32, storyPoint, "story_point") = 0;
    DTO_FIELD(Boolean, finished, "finished") = false;
    DTO_FIELD(String, name, "name") = "";
    DTO_FIELD(String, ownerName, "owner_name") = "";
    DTO_FIELD(String, ownerEmail, "owner_email") = "";
};

class UpdateTasksDto : public oatpp::DTO
{
    DTO_INIT(UpdateTasksDto, DTO)

    DTO_FIELD(Vector<Object<TaskDto>>, create, "create") = nullptr;
    DTO_FIELD(Vector<Object<TaskDto>>, update, "update") = nullptr;
    DTO_FIELD(Vector<Object<TaskDto>>, deleted, "deleted") = nullptr;
};

#include OATPP_CODEGEN_END(DTO)