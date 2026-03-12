#pragma once

#ifndef DATABASE_MIGRATIONS
#define DATABASE_MIGRATIONS "sql"
#endif

#include "dto/UserDto.hpp"
#include "dto/SprintDto.hpp"
#include "oatpp-postgresql/orm.hpp"

#include "oatpp/base/Log.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient) //<- Begin Codegen

using namespace oatpp;

class SprintDb : public oatpp::orm::DbClient
{
public:
    SprintDb(const std::shared_ptr<oatpp::orm::Executor> &executor)
        : oatpp::orm::DbClient(executor)
    {
    }
    QUERY(getSprintsInBetweenDates,
          "SELECT * FROM get_sprints_in_between_dates(:projectId, :startDate, :endDate)",
          PREPARE(true),
          PARAM(Int32, projectId),
          PARAM(String, startDate),
          PARAM(String, endDate));

    QUERY(createSprint,
          "SELECT * FROM create_sprint(:dto.project_id, :dto.name, :dto.goal, :dto.start_date)",
          PREPARE(true),
          PARAM(Object<CreateSprintDto>, dto));

    QUERY(getMemberByUserIdAndSprintId,
          "SELECT m.user_id, m.project_id, m.role FROM Member m "
          "INNER JOIN Sprint s ON s.project_id = m.project_id "
          "WHERE m.user_id = :userId AND s.id = :sprintId",
          PREPARE(true),
          PARAM(Int32, userId),
          PARAM(Int32, sprintId));

    QUERY(createSprintBackLog,
          "SELECT create_sprint_backlog(:dto.sprint_id, :dto.product_backlog_id, :dto.task_owner, "
          ":dto.note, :dto.estimated_story_point);",
          PREPARE(true),
          PARAM(Object<CreateSprintBacklogDto>, dto));
};

#include OATPP_CODEGEN_END(DbClient) //<- End Codegen