#pragma once

#ifndef DATABASE_MIGRATIONS
#define DATABASE_MIGRATIONS "sql"
#endif

#include "dto/UserDto.hpp"
#include "dto/SprintDto.hpp"
#include "oatpp-postgresql/orm.hpp"
#include "oatpp/web/protocol/http/Http.hpp"

#include "oatpp/base/Log.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient) //<- Begin Codegen

struct PO
{
};
struct EM
{
};
struct SM
{
};
struct POSM
{
};
struct Pid
{
};
struct Sid
{
};
struct Bid
{
};

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

      QUERY(getMember,
            "SELECT * FROM Member WHERE project_id = :project_id AND user_id = :user_id",
            PREPARE(true),
            PARAM(oatpp::Int32, user_id),
            PARAM(oatpp::Int32, project_id));

      QUERY(getMemberByUserIdAndSprintId,
            "SELECT m.user_id, m.project_id, m.role FROM Member m "
            "INNER JOIN Sprint s ON s.project_id = m.project_id "
            "WHERE m.user_id = :userId AND s.id = :sprintId",
            PREPARE(true),
            PARAM(Int32, userId),
            PARAM(Int32, sprintId));

      QUERY(getMemberByUserIdAndSprintBacklogId,
            "SELECT m.user_id, m.project_id, m.role FROM SprintBacklog sb "
            "INNER JOIN Sprint s ON s.id = sb.sprint_id "
            "INNER JOIN Member m ON s.project_id = m.project_id AND m.user_id = :userId "
            "WHERE sb.id = :backlogId",
            PREPARE(true),
            PARAM(Int32, userId),
            PARAM(Int32, backlogId));

      QUERY(createSprintBackLog,
            "SELECT create_sprint_backlog(:dto.sprint_id, :dto.product_backlog_id, :dto.task_owner, "
            ":dto.note);",
            PREPARE(true),
            PARAM(Object<CreateSprintBacklogDto>, dto));

      QUERY(getSprintBacklogBySprintId,
            "SELECT * FROM get_sprint_backlog_by_sprint_id(:sprintId);",
            PREPARE(true),
            PARAM(Int32, sprintId));

      template <typename role, typename id>
      void checkMemberExist(const Int32 &userId, const Int32 &programId)
      {
            std::shared_ptr<oatpp::orm::QueryResult> dbResult;
            if constexpr (std::is_same_v<id, Pid>)
                  dbResult = this->getMember(userId, programId);
            if constexpr (std::is_same_v<id, Bid>)
                  dbResult = this->getMemberByUserIdAndSprintBacklogId(userId, programId);
            if constexpr (std::is_same_v<id, Sid>)
                  dbResult = this->getMemberByUserIdAndSprintId(userId, programId);
            OATPP_ASSERT_HTTP(dbResult->isSuccess(), oatpp::web::protocol::http::Status::CODE_500, dbResult->getErrorMessage());
            auto members = dbResult->fetch<Vector<Object<MemberDto>>>();
            OATPP_ASSERT_HTTP(members->size() == 1, oatpp::web::protocol::http::Status::CODE_400, "User does not belong to the project");
            if constexpr (std::is_same_v<role, EM>)
                  return;
            if constexpr (std::is_same_v<role, PO>)
                  OATPP_ASSERT_HTTP(members[0]->role.equalsCI_ASCII("PO"), oatpp::web::protocol::http::Status::CODE_400, "Only PO can have authority for this action");
            if constexpr (std::is_same_v<role, SM>)
                  OATPP_ASSERT_HTTP(members[0]->role.equalsCI_ASCII("SM"), oatpp::web::protocol::http::Status::CODE_400, "Only SM can have authority for this action");
            if constexpr (std::is_same_v<role, POSM>)
                  OATPP_ASSERT_HTTP(!members[0]->role.equalsCI_ASCII("EM"), oatpp::web::protocol::http::Status::CODE_400, "Only SM or PO can have authority for this action");
      };

      QUERY(deleteBacklogById,
            "DELETE FROM SprintBacklog sb WHERE id = :backlogId AND sb.sprint_id = :sprintId",
            PREPARE(true),
            PARAM(Int32, backlogId),
            PARAM(Int32, sprintId));

      QUERY(updateSprintBacklog,
            "SELECT update_sprint_backlog(:dto.id, :dto.taskOwner, :dto.status, :dto.notes)",
            PREPARE(true),
            PARAM(Object<UpdateSprintBacklogDto>, dto));

      QUERY(createTask,
            "INSERT INTO Task (user_id, sprint_backlog_id, story_point, finished, name) VALUES "
            "(:dto.user_id, :dto.sprint_backlog_id, :dto.story_point, :dto.finished, :dto.name);",
            PREPARE(true),
            PARAM(Object<TaskDto>, dto));

      QUERY(updateTask,
            "UPDATE Task SET "
            "  user_id = :dto.user_id, "
            "  story_point = :dto.story_point, "
            "  finished = CAST(:dto.finished AS BOOLEAN), "
            "  name = :dto.name "
            "WHERE id = :dto.id AND sprint_backlog_id = :dto.sprint_backlog_id;",
            PREPARE(true),
            PARAM(Object<TaskDto>, dto));

      // DELETE: Deletes based on both ID and Sprint Backlog ID for extra safety.
      QUERY(deleteTask,
            "DELETE FROM Task "
            "WHERE id = :dto.id AND sprint_backlog_id = :dto.sprint_backlog_id;",
            PREPARE(true),
            PARAM(Object<TaskDto>, dto));
};

#include OATPP_CODEGEN_END(DbClient) //<- End Codegen