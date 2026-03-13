#include "SprintService.hpp"

#define CHECK_SUCCESS OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());

void SprintService::createSprint(const Int32 &userId, const Object<CreateSprintDto> &sprint)
{
    m_sprintDatabase->checkMemberExist<POSM, Pid>(userId, sprint->projectId);
    auto dbResult = m_sprintDatabase->createSprint(sprint);
    CHECK_SUCCESS;
}

Vector<Object<SprintDto>> SprintService::getSprints(const Int32 &userId, const Int32 &projectId, const String &start, const String &end)
{
    m_sprintDatabase->checkMemberExist<EM, Pid>(userId, projectId);
    auto dbResult = m_sprintDatabase->getSprintsInBetweenDates(projectId, start, end);
    CHECK_SUCCESS;
    return dbResult->fetch<Vector<Object<SprintDto>>>();
}

void SprintService::createSprintBacklog(const Int32 &userId, const Object<CreateSprintBacklogDto> &dto)
{
    m_sprintDatabase->checkMemberExist<POSM, Sid>(userId, dto->sprintId);
    if (dto->taskOwner)
    {
        m_sprintDatabase->checkMemberExist<EM, Sid>(dto->taskOwner, dto->sprintId);
    }
    auto dbResult = m_sprintDatabase->createSprintBackLog(dto);
    CHECK_SUCCESS;
}

Vector<Object<SprintBacklogDetailDto>> SprintService::getSprintBacklogs(const Int32 &userId, const Int32 &sprintId)
{
    m_sprintDatabase->checkMemberExist<EM, Sid>(userId, sprintId);
    auto dbResult = m_sprintDatabase->getSprintBacklogBySprintId(sprintId);
    CHECK_SUCCESS;
    return dbResult->fetch<Vector<Object<SprintBacklogDetailDto>>>();
}

void SprintService::deleteSprintBacklog(const Int32 &userId, const Int32 &sprintId, const Int32 &sprintBacklogId)
{
    m_sprintDatabase->checkMemberExist<POSM, Sid>(userId, sprintId);
    auto dbResult = m_sprintDatabase->deleteBacklogById(sprintBacklogId, sprintId);
    CHECK_SUCCESS;
}

void SprintService::updateSprintBacklog(const Int32 &userId, const Object<UpdateSprintBacklogDto> &sprintBacklog)
{
    if (sprintBacklog->status.equalsCI_ASCII("finished") || sprintBacklog->status.equalsCI_ASCII("failed"))
    {
        m_sprintDatabase->checkMemberExist<POSM, Bid>(userId, sprintBacklog->id);
    }
    if (sprintBacklog->taskOwner)
    {
        m_sprintDatabase->checkMemberExist<EM, Bid>(sprintBacklog->taskOwner, sprintBacklog->id);
        m_sprintDatabase->checkMemberExist<POSM, Bid>(userId, sprintBacklog->id);
    }
    else
    {
        m_sprintDatabase->checkMemberExist<EM, Bid>(userId, sprintBacklog->id);
    }
    auto dbResult = m_sprintDatabase->updateSprintBacklog(sprintBacklog);
    CHECK_SUCCESS;
}
