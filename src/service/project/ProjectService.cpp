#include "ProjectService.hpp"

#define CHECK_SUCCESS OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());

void ProjectService::createProject(const oatpp::Object<ProjectDto> &project)
{
    auto dbResult = this->m_projectDatabase->createProject(project);
    CHECK_SUCCESS
}
