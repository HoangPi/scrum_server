#pragma once

#include "auth/AuthHandler.hpp"
#include "dto/AuthDto.hpp"

#include "db/UserDb.hpp"
#include "db/ProjectDb.hpp"
#include "db/SprintDb.hpp"

#include "dto/PageDto.hpp"
#include "dto/StatusDto.hpp"
#include "dto/ProjectDto.hpp"
#include "dto/SprintDto.hpp"

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/macro/component.hpp"

using namespace oatpp;

class SprintService
{
private:
    typedef oatpp::web::protocol::http::Status Status;

private:
    OATPP_COMPONENT(std::shared_ptr<UserDb>, m_userDatabase);       // Inject database component
    OATPP_COMPONENT(std::shared_ptr<ProjectDb>, m_projectDatabase); // Inject database component
    OATPP_COMPONENT(std::shared_ptr<SprintDb>, m_sprintDatabase); // Inject database component
public:
    void createSprint(const Int32 &userId, const Object<CreateSprintDto> &sprint);
    Vector<Object<SprintDto>> getSprints(const Int32 &userId, const Int32 &projectId, const String &start, const String &end);
    void createSprintBacklog(const Int32 &userId, const Object<CreateSprintBacklogDto> &dto);
};