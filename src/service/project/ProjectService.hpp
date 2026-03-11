#pragma once

#include "auth/AuthHandler.hpp"
#include "dto/AuthDto.hpp"

#include "db/UserDb.hpp"
#include "db/ProjectDb.hpp"

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
    OATPP_COMPONENT(std::shared_ptr<UserDb>, m_userDatabase); // Inject database component
    OATPP_COMPONENT(std::shared_ptr<ProjectDb>, m_projectDatabase); // Inject database component
public:
    void createProject(const oatpp::Object<ProjectDto> &project);
    Vector<Object<ProjectDto>> getProjects(const Int32 &userId);
   Object<ProjectDto> getProjectById(const Int32 &projectId);
};