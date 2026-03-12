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

      QUERY(deleteInvite,
            "DELETE FROM invite WHERE project_id = :dto.project_id AND invitee = :dto.invitee AND status = 'created'; ",
            PREPARE(false),
            PARAM(oatpp::Object<InviteDto>, dto));

      QUERY(createInvite,
            "INSERT INTO invite (invitee, project_id) VALUES (:dto.invitee, :dto.project_id);",
            PREPARE(false),
            PARAM(oatpp::Object<InviteDto>, dto));

      QUERY(updateInvite,
            "SELECT * FROM update_invite(:inviteId, :userId, :accept)",
            PREPARE(true),
            PARAM(oatpp::Int32, userId),
            PARAM(oatpp::Int32, inviteId),
            PARAM(oatpp::Boolean, accept));

      QUERY(getMember,
            "SELECT * FROM Member WHERE project_id = :project_id AND user_id = :user_id",
            PREPARE(true),
            PARAM(oatpp::Int32, user_id),
            PARAM(oatpp::Int32, project_id));
};

#include OATPP_CODEGEN_END(DbClient) //<- End Codegen