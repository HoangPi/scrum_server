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

      QUERY(createProductBacklog,
            "INSERT INTO ProductBacklog "
            "(project_id, name, acceptance_criteria, priority, story_point) "
            "VALUES "
            "(:dto.project_id, :dto.name, :dto.acceptance_criteria, :dto.priority, :dto.story_point) "
            "RETURNING *;",
            PREPARE(true),
            PARAM(oatpp::Object<ProductBacklogDto>, dto));

      QUERY(getProductBacklogs,
            "SELECT * FROM get_product_backlog(:projectId, :offset, :includeFinished)",
            PREPARE(true),
            PARAM(oatpp::Int32, projectId),
            PARAM(oatpp::Int32, offset),
            PARAM(oatpp::Boolean, includeFinished));
      QUERY(getProductBacklogsWithQuery,
            "SELECT * FROM get_product_backlog_with_query(:projectId, :offset, :nameFilter, :finished, :storyPoint, :priority)",
            PREPARE(true),
            PARAM(oatpp::Int32, projectId),
            PARAM(oatpp::Int32, offset),
            PARAM(oatpp::String, nameFilter),
            PARAM(oatpp::Boolean, finished),
            PARAM(oatpp::Boolean, storyPoint),
            PARAM(oatpp::Boolean, priority));

      QUERY(getUserByProjectIdAndEmail,
            "SELECT u.id, name, email FROM Member m "
            "INNER JOIN AppUser u ON u.id = m.user_id "
            "WHERE u.email ILIKE :likeEmail AND m.project_id = :projectId LIMIT 6",
            PREPARE(false),
            PARAM(oatpp::Int32, projectId),
            PARAM(oatpp::String, likeEmail));

      QUERY(updateProductBacklog,
            "UPDATE ProductBacklog SET name = :backlog.name, acceptance_criteria = :backlog.acceptance_criteria, "
            "priority = :backlog.priority, story_point = :backlog.story_point WHERE id = :backlog.id",
            PREPARE(true),
            PARAM(oatpp::Object<ProductBacklogDto>, backlog));

      QUERY(getManagersOfProject,
            "SELECT id, name, email, member.role FROM Member "
            "INNER JOIN AppUser ON Member.user_id = AppUser.id "
            "WHERE project_id = :projectId AND member.role IN ('PO', 'SM')",
            PREPARE(true),
            PARAM(oatpp::Int32, projectId));

      QUERY(getEmployeesOfProject,
            "SELECT id, name, email, member.role FROM Member "
            "INNER JOIN AppUser ON Member.user_id = AppUser.id "
            "WHERE project_id = :projectId AND member.role = 'EM' ORDER BY id ASC LIMIT 20 OFFSET (20 * :offset)",
            PREPARE(true),
            PARAM(oatpp::Int32, projectId),
            PARAM(oatpp::Int32, offset));

      QUERY(getProjectOverView,
            "SELECT * FROM get_project_overview(:projectId)",
            PREPARE(true),
            PARAM(oatpp::Int32, projectId));

      QUERY(updateRole,
            "UPDATE Member SET role = :newRole WHERE user_id = :userId AND project_id = :projectId",
            PREPARE(true),
            PARAM(oatpp::Int32, userId),
            PARAM(oatpp::Int32, projectId),
            PARAM(oatpp::String, newRole));
};

#include OATPP_CODEGEN_END(DbClient) //<- End Codegen