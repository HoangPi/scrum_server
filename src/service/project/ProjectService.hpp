#pragma once

#include "auth/AuthHandler.hpp"
#include "dto/AuthDto.hpp"

#include "db/UserDb.hpp"
#include "db/ProjectDb.hpp"
#include "db/SprintDb.hpp"

#include "dto/PageDto.hpp"
#include "dto/StatusDto.hpp"
#include "dto/ProjectDto.hpp"

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/macro/component.hpp"

using namespace oatpp;

class ProjectService
{
private:
    typedef oatpp::web::protocol::http::Status Status;

private:
    OATPP_COMPONENT(std::shared_ptr<UserDb>, m_userDatabase);       // Inject database component
    OATPP_COMPONENT(std::shared_ptr<ProjectDb>, m_projectDatabase); // Inject database component
    OATPP_COMPONENT(std::shared_ptr<SprintDb>, m_sprintDatabase);

public:
    void createProject(const oatpp::Object<ProjectDto> &project);
    Vector<Object<ProjectDto>> getProjects(const Int32 &userId);
    Object<ProjectDto> getProjectById(const Int32 &projectId);
    void createInvite(const Object<InviteDto> &invite, const Int32 &userId);
    void createManyInvites(const Object<InviteManyDto> &invite, const Int32 &userId);
    void updateInvite(const Int32 &userId, const Int32 &inviteId, const Boolean &accept);
    void createProductBacklog(const Int32 &userId, const Object<ProductBacklogDto> &backlog);
    Vector<Object<ProductBacklogDto>> getProductBacklogs(const Int32 &userId, const Int32 &projectId, const Int32 &offset, const Boolean &includeFinished);
    Vector<Object<ProductBacklogDto>> getProductBacklogsWithQuery(const Int32 &userId,
                                                                  const Int32 &projectId,
                                                                  const Int32 &offset, const String &nameFilter,
                                                                  const Boolean &finished, const Boolean &storyPointAsc,
                                                                  const Boolean &priorityAsc);
    Vector<Object<MemberInfo>> getMemberByEmailAndProjectId(const Int32 &userId, const String &email, const Int32 &projectId);
    Int32 getBacklogCount(const Int32 &userId, const Int32 &projectId);
    void updateProductBacklog(const Int32 &userId, const Object<ProductBacklogDto> &backlog);
    void deleteProjectById(const Int32 &userId, const Int32 &projectId);
    Vector<Object<MemberInfoWithRoleDto>> getManagersOfProject(const Int32 &userId, const Int32 &projectId);
    Vector<Object<MemberInfoWithRoleDto>> getEmployeesOfProject(const Int32 &userId, const Int32 &projectId, const Int32 &offset);
    Vector<Object<ProjectOverviewDto>> getProjectOverView(const Int32 &userId, const Int32 &projectId);
};