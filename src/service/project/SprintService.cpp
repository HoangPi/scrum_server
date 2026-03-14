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
    m_sprintDatabase->checkMemberExist<POSM, Bid>(userId, sprintBacklogId);
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

void SprintService::bulkUpdateTasks(const Int32 &userId, const Object<UpdateTasksDto> &tasks)
{
    auto transaction = m_sprintDatabase->beginTransaction();
    std::shared_ptr<oatpp::orm::QueryResult> dbResult;
    try
    {
        std::vector<pair> pairs;
        if (tasks->create)
        {
            for (size_t i = 0; i < tasks->create->size(); i++)
            {
                auto &item = tasks->create[i];
                // Check Task Owner + Backlog pair
                const pair p1 = {*item->taskOwner, *item->sprintBacklogId};
                if (!checkPairExists(pairs, p1))
                {
                    m_sprintDatabase->checkMemberExist<EM, Bid>(item->taskOwner, item->sprintBacklogId);
                    pairs.push_back(p1);
                }

                // Check Current User + Backlog pair (Verify requester has access)
                const pair p2 = {*userId, *item->sprintBacklogId};
                if (!checkPairExists(pairs, p2))
                {
                    m_sprintDatabase->checkMemberExist<EM, Bid>(userId, item->sprintBacklogId);
                    pairs.push_back(p2);
                }

                dbResult = m_sprintDatabase->createTask(item, transaction.getConnection());
                CHECK_SUCCESS;
            }
        }

        // 2. HANDLE UPDATES
        if (tasks->update)
        {
            for (size_t i = 0; i < tasks->update->size(); i++)
            {
                auto &item = tasks->update[i];
                // Validate pairs for update (same logic as create)
                const pair p1 = {*item->taskOwner, *item->sprintBacklogId};
                if (!checkPairExists(pairs, p1))
                {
                    m_sprintDatabase->checkMemberExist<EM, Bid>(item->taskOwner, item->sprintBacklogId);
                    pairs.push_back(p1);
                }
                const pair p2 = {*userId, *item->sprintBacklogId};
                if (!checkPairExists(pairs, p2))
                {
                    m_sprintDatabase->checkMemberExist<EM, Bid>(userId, item->sprintBacklogId);
                    pairs.push_back(p2);
                }

                dbResult = m_sprintDatabase->updateTask(item, transaction.getConnection());
                CHECK_SUCCESS;
            }
        }
        // 3. HANDLE DELETES
        if (tasks->deleted)
        {
            for (size_t i = 0; i < tasks->deleted->size(); i++)
            {
                auto &item = tasks->deleted[i];
                // For delete, we usually just need to verify the requester has access to the backlog
                pair p = {*userId, *item->sprintBacklogId};
                if (!checkPairExists(pairs, p))
                {
                    m_sprintDatabase->checkMemberExist<EM, Bid>(userId, item->sprintBacklogId);
                    pairs.push_back(p);
                }

                dbResult = m_sprintDatabase->deleteTask(item, transaction.getConnection());
                CHECK_SUCCESS;
            }
        }
        transaction.commit();
    }
    catch (...)
    {
        transaction.rollback();
        throw;
    }
}
