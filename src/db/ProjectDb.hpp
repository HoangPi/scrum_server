#pragma once

#include "oatpp-postgresql/orm.hpp"
#include "dto/ProjectDto.hpp"

#include "oatpp/base/Log.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient) //<- Begin Codegen

/**
 * UserDb client definitions.
 */
class ProjectDb : public oatpp::orm::DbClient
{
public:
    ProjectDb(const std::shared_ptr<oatpp::orm::Executor> &executor)
        : oatpp::orm::DbClient(executor) {
          };

    QUERY(createProject,
          "INSERT INTO Project "
          "(name, project_owner, description, sprint_length, sprint_start_day, work_day_hour, default_day_off) "
          "VALUES (:project.name, :project.project_owner, :project.description, :project.sprint_length, "
          ":project.sprint_start_day, :project.work_day_hour, :project.default_day_off);",
          PREPARE(true),
          PARAM(oatpp::Object<ProjectDto>, project));

    QUERY(getProjectsByUserId,
          "SELECT id, project_owner, name, description, sprint_length, sprint_start_day, work_day_hour, default_day_off, status "
          "FROM Member m "
          "INNER JOIN Project p ON p.id = m.project_id "
          "WHERE m.user_id = :userId",
          PREPARE(true),
          PARAM(oatpp::Int32, userId));

    QUERY(getProjectById,
          "SELECT p.id, project_owner, name, description, sprint_length, sprint_start_day, work_day_hour, default_day_off, status "
          "FROM Project p "
          "WHERE p.id = :projectId",
          PREPARE(true),
          PARAM(oatpp::Int32, projectId));
};

#include OATPP_CODEGEN_END(DbClient) //<- End Codegen