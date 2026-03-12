#include "SprintService.hpp"

#define CHECK_SUCCESS OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());

void SprintService::createSprint(const Int32 &userId, const Object<CreateSprintDto> &sprint)
{
    auto dbResult = m_projectDatabase->getMember(userId, sprint->projectId);
    CHECK_SUCCESS;
    auto members = dbResult->fetch<Vector<Object<MemberDto>>>();
    OATPP_ASSERT_HTTP(members->size() == 1, Status::CODE_400, "User does not belong to the project");
    OATPP_ASSERT_HTTP(!members[0]->role.equalsCI_ASCII("EM"), Status::CODE_400, "Only SM and PO has the authority to create a sprint");
    dbResult = m_sprintDatabase->createSprint(sprint);
    CHECK_SUCCESS;
}

Vector<Object<SprintDto>> SprintService::getSprints(const Int32 &userId, const Int32 &projectId, const String &start, const String &end)
{
    auto dbResult = m_projectDatabase->getMember(userId, projectId);
    CHECK_SUCCESS;
    auto members = dbResult->fetch<Vector<Object<MemberDto>>>();
    OATPP_ASSERT_HTTP(members->size() == 1, Status::CODE_400, "User does not belong to the project");
    dbResult = m_sprintDatabase->getSprintsInBetweenDates(projectId, start, end);
    CHECK_SUCCESS;
    return dbResult->fetch<Vector<Object<SprintDto>>>();
}

void SprintService::createSprintBacklog(const Int32 &userId, const Object<CreateSprintBacklogDto> &dto)
{
    auto dbResult = m_sprintDatabase->getMemberByUserIdAndSprintId(userId, dto->sprintId);
    CHECK_SUCCESS;
    auto members = dbResult->fetch<Vector<Object<MemberDto>>>();
    OATPP_ASSERT_HTTP(members->size() == 1, Status::CODE_400, "User does not belong to the project");
    OATPP_ASSERT_HTTP(!members[0]->role.equalsCI_ASCII("EM"), Status::CODE_400, "Only SM and PO has the authority to create sprint backlog");

    if (dto->taskOwner)
    {
        dbResult = m_sprintDatabase->getMemberByUserIdAndSprintId(dto->taskOwner, dto->sprintId);
        CHECK_SUCCESS;
        members = dbResult->fetch<Vector<Object<MemberDto>>>();
        OATPP_ASSERT_HTTP(members->size() == 1, Status::CODE_400, "User does not belong to the project");
    }
    dbResult = m_sprintDatabase->createSprintBackLog(dto);
    CHECK_SUCCESS;
}
