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
