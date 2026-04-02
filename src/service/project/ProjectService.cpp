#include "ProjectService.hpp"

const std::string daysOfWeek[7] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

#define CHECK_SUCCESS OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());

void ProjectService::createProject(const oatpp::Object<ProjectDto> &project)
{
    auto dbResult = this->m_projectDatabase->createProject(project);
    CHECK_SUCCESS
}

Vector<Object<ProjectDto>> ProjectService::getProjects(const Int32 &userId)
{
    auto dbResult = m_projectDatabase->getProjectsByUserId(userId);
    CHECK_SUCCESS;
    auto projects = dbResult->fetch<Vector<Object<ProjectDto>>>();
    for (size_t i = 0; i < projects->size(); i++)
    {
        const auto offDays = *projects[i]->defaultDayOff.get();
        projects[i]->dayOff = Vector<String>::createShared();
        projects[i]->dayOff->reserve(2);
        for (int d = 0; d < 7; d++)
        {
            if (offDays & (1 << d))
            {
                projects[i]->dayOff->push_back(daysOfWeek[d]);
            }
        }
    }
    return projects;
}

Object<ProjectDto> ProjectService::getProjectById(const Int32 &projectId)
{
    auto dbResult = m_projectDatabase->getProjectById(projectId);
    CHECK_SUCCESS;
    auto projects = dbResult->fetch<Vector<Object<ProjectDto>>>();
    OATPP_ASSERT_HTTP(projects->size(), Status::CODE_404, "Project not found");
    const auto offDays = *projects[0]->defaultDayOff.get();
    projects[0]->dayOff = Vector<String>::createShared();
    projects[0]->dayOff->reserve(2);
    for (int d = 0; d < 7; d++)
    {
        if (offDays & (1 << d))
        {
            projects[0]->dayOff->push_back(daysOfWeek[d]);
        }
    }
    return projects[0];
}

void ProjectService::createInvite(const Object<InviteDto> &invite, const Int32 &userId)
{
    auto dbResult = m_projectDatabase->getMember(userId, invite->projectId);
    CHECK_SUCCESS;
    auto members = dbResult->fetch<Vector<Object<MemberDto>>>();
    OATPP_ASSERT_HTTP(members->size() == 1, Status::CODE_400, "User is not a member of project");
    OATPP_ASSERT_HTTP(!members[0]->role.equalsCI_ASCII("EM"), Status::CODE_400, "Employee cannot send invite");
    dbResult = m_projectDatabase->getMember(invite->invitee, invite->projectId);
    CHECK_SUCCESS;
    members = dbResult->fetch<Vector<Object<MemberDto>>>();
    OATPP_ASSERT_HTTP(members->size() == 0, Status::CODE_400, "User has been a member of project already")
    dbResult = m_projectDatabase->deleteInvite(invite);
    CHECK_SUCCESS;
    dbResult = m_projectDatabase->createInvite(invite);
    CHECK_SUCCESS;
}

void ProjectService::createManyInvites(const Object<InviteManyDto> &invite, const Int32 &userId)
{
    auto transaction = m_projectDatabase->beginTransaction();
    auto singleInvite = Object<InviteDto>::createShared();
    singleInvite->projectId = invite->projectId;
    std::shared_ptr<oatpp::orm::QueryResult> dbResult;
    try
    {
        m_sprintDatabase->checkMemberExist<POSM, Pid>(userId, invite->projectId);
        for (size_t i = 0; i < invite->invitees->size(); i++)
        {
            singleInvite->invitee = invite->invitees[i];
            dbResult = m_projectDatabase->getMember(singleInvite->invitee, singleInvite->projectId, transaction.getConnection());
            CHECK_SUCCESS;
            auto members = dbResult->fetch<Vector<Object<MemberDto>>>();
            OATPP_ASSERT_HTTP(members->size() == 0, Status::CODE_400, "User has been a member of project already")
            dbResult = m_projectDatabase->deleteInvite(singleInvite, transaction.getConnection());
            CHECK_SUCCESS;
            dbResult = m_projectDatabase->createInvite(singleInvite, transaction.getConnection());
            CHECK_SUCCESS;
        }
    }
    catch (...)
    {
        transaction.rollback();
        throw;
    }
    transaction.commit();
}

void ProjectService::updateInvite(const Int32 &userId, const Int32 &inviteId, const Boolean &accept)
{
    auto dbResult = m_projectDatabase->updateInvite(userId, inviteId, accept);
    CHECK_SUCCESS;
}

void ProjectService::createProductBacklog(const Int32 &userId, const Object<ProductBacklogDto> &backlog)
{
    auto dbResult = m_projectDatabase->getMember(userId, backlog->projectId);
    CHECK_SUCCESS;
    auto members = dbResult->fetch<Vector<Object<MemberDto>>>();
    OATPP_ASSERT_HTTP(members->size() == 1, Status::CODE_400, "User does not belong to the project");
    OATPP_ASSERT_HTTP(!members[0]->role.equalsCI_ASCII("EM"), Status::CODE_400, "Only SM and PO has the authority to create product backlog");
    dbResult = m_projectDatabase->createProductBacklog(backlog);
    CHECK_SUCCESS;
}

Vector<Object<ProductBacklogDto>> ProjectService::getProductBacklogs(const Int32 &userId, const Int32 &projectId, const Int32 &offset, const Boolean &includeFinished)
{
    auto dbResult = m_projectDatabase->getMember(userId, projectId);
    CHECK_SUCCESS;
    auto members = dbResult->fetch<Vector<Object<MemberDto>>>();
    OATPP_ASSERT_HTTP(members->size() == 1, Status::CODE_400, "User does not belong to the project");

    dbResult = m_projectDatabase->getProductBacklogs(projectId, offset, includeFinished);
    CHECK_SUCCESS;
    return dbResult->fetch<Vector<Object<ProductBacklogDto>>>();
}

Vector<Object<ProductBacklogDto>> ProjectService::getProductBacklogsWithQuery(const Int32 &userId, const Int32 &projectId, const Int32 &offset, const String &nameFilter, const Boolean &finished, const Boolean &storyPointAsc, const Boolean &priorityAsc)
{
    auto dbResult = m_projectDatabase->getMember(userId, projectId);
    CHECK_SUCCESS;
    auto members = dbResult->fetch<Vector<Object<MemberDto>>>();
    OATPP_ASSERT_HTTP(members->size() == 1, Status::CODE_400, "User does not belong to the project");
    dbResult = m_projectDatabase->getProductBacklogsWithQuery(projectId, offset, nameFilter, finished, storyPointAsc, priorityAsc);
    CHECK_SUCCESS;
    return dbResult->fetch<Vector<Object<ProductBacklogDto>>>();
}

Vector<Object<MemberInfo>> ProjectService::getMemberByEmailAndProjectId(const Int32 &userId, const String &email, const Int32 &projectId)
{
    m_sprintDatabase->checkMemberExist<EM, Pid>(userId, projectId);
    String likeEmail = "%" + email + "%";
    auto dbResult = m_projectDatabase->getUserByProjectIdAndEmail(projectId, likeEmail);
    CHECK_SUCCESS;
    return dbResult->fetch<Vector<Object<MemberInfo>>>();
}

Int32 ProjectService::getBacklogCount(const Int32 &userId, const Int32 &projectId)
{
    m_sprintDatabase->checkMemberExist<EM, Pid>(userId, projectId);
    auto dbResult = m_projectDatabase->executeQuery("SELECT COUNT(project_id) AS id FROM ProductBacklog WHERE project_id = :projectId GROUP BY project_id", {{"projectId", projectId}});
    CHECK_SUCCESS;
    auto ids = dbResult->fetch<Vector<Object<IdDto>>>();
    OATPP_ASSERT_HTTP(ids->size() == 1, Status::CODE_404, "Project not found");
    return ids[0]->id;
}

void ProjectService::updateProductBacklog(const Int32 &userId, const Object<ProductBacklogDto> &backlog)
{
    m_sprintDatabase->checkMemberExist<POSM, Pid>(userId, backlog->projectId);
    auto dbResult = m_projectDatabase->updateProductBacklog(backlog);
    CHECK_SUCCESS;
    return;
}

void ProjectService::deleteProjectById(const Int32 &userId, const Int32 &projectId)
{
    m_sprintDatabase->checkMemberExist<PO, Pid>(userId, projectId);
    auto dbResult = m_projectDatabase->executeQuery(
        "DELETE FROM Project WHERE id = :project_id "
        "AND project_owner = :owner",
        {{"owner", userId},
         {"project_id", projectId}});
    CHECK_SUCCESS;
}

Vector<Object<MemberInfoWithRoleDto>> ProjectService::getManagersOfProject(const Int32 &userId, const Int32 &projectId)
{
    m_sprintDatabase->checkMemberExist<EM, Pid>(userId, projectId);
    auto dbResult = m_projectDatabase->getManagersOfProject(projectId);
    CHECK_SUCCESS;
    return dbResult->fetch<Vector<Object<MemberInfoWithRoleDto>>>();
}

Vector<Object<MemberInfoWithRoleDto>> ProjectService::getEmployeesOfProject(const Int32 &userId, const Int32 &projectId, const Int32 &offset)
{
    m_sprintDatabase->checkMemberExist<EM, Pid>(userId, projectId);
    auto dbResult = m_projectDatabase->getEmployeesOfProject(projectId, offset);
    CHECK_SUCCESS;
    return dbResult->fetch<Vector<Object<MemberInfoWithRoleDto>>>();
}

Vector<Object<ProjectOverviewDto>> ProjectService::getProjectOverView(const Int32 &userId, const Int32 &projectId)
{
    m_sprintDatabase->checkMemberExist<EM, Pid>(userId, projectId);
    auto dbResult = m_projectDatabase->getProjectOverView(projectId);
    CHECK_SUCCESS;
    return dbResult->fetch<Vector<Object<ProjectOverviewDto>>>();
}

void ProjectService::updateRole(const Int32 &userId, const Int32 &projectId, const Int32 &memberId, const String &newRole)
{
    m_sprintDatabase->checkMemberExist<PO, Pid>(userId, projectId);
    m_sprintDatabase->checkMemberExist<EM, Pid>(memberId, projectId);
    auto dbResult = m_projectDatabase->updateRole(memberId, projectId, newRole);
    CHECK_SUCCESS;
}
