#pragma once

#include "service/UserService.hpp"
#include "service/project/ProjectService.hpp"
#include "service/project/SprintService.hpp"
#include "dto/SprintDto.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/web/mime/ContentMappers.hpp"
#include "oatpp/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

/**
 * User REST controller.
 */
class SprintController : public oatpp::web::server::api::ApiController
{
public:
    SprintController(OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
        : oatpp::web::server::api::ApiController(apiContentMappers)
    {
    }

private:
    UserService m_userService;       // Create user service.
    ProjectService m_projectService; // Create user service.
    SprintService m_sprintService;   // Create user service.
public:
    static std::shared_ptr<SprintController> createShared(
        OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers) // Inject ContentMappers
    )
    {
        return std::make_shared<SprintController>(apiContentMappers);
    }

    ENDPOINT_INFO(getSprints)
    {
        info->summary = "Create new User";

        info->addConsumes<Object<UserDto>>("application/json");

        info->addResponse<Object<ReturnUserDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("GET", "sprints", getSprints,
             QUERY(String, end),
             QUERY(String, start),
             QUERY(Int32, projectId),
             REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        return createDtoResponse(Status::CODE_200, m_sprintService.getSprints(int(request->getBundleData<Int64>("userId")), projectId, start, end));
    }

    ENDPOINT_INFO(createSprint)
    {
        info->summary = "Create new User";

        info->addConsumes<Object<UserDto>>("application/json");

        info->addResponse<Object<ReturnUserDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("POST", "sprints", createSprint,
             BODY_DTO(Object<CreateSprintDto>, dto),
             REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        m_sprintService.createSprint(int(request->getBundleData<Int64>("userId")), dto);
        return createResponse(Status::CODE_200, nullptr);
    }

    ENDPOINT_INFO(createSprintBacklog)
    {
        info->summary = "Create new User";

        info->addConsumes<Object<UserDto>>("application/json");

        info->addResponse<Object<ReturnUserDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("POST", "sprints/backlog", createSprintBacklog,
             BODY_DTO(Object<CreateSprintBacklogDto>, dto),
             REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        m_sprintService.createSprintBacklog(int(request->getBundleData<Int64>("userId")), dto);
        return createResponse(Status::CODE_200, nullptr);
    }

    ENDPOINT_INFO(removeSprintBacklog)
    {
        info->summary = "Create new User";

        info->addConsumes<Object<UserDto>>("application/json");

        info->addResponse<Object<ReturnUserDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("DELETE", "sprints/${sprintId}/${sprintBackLogId}", removeSprintBacklog,
             PATH(Int32, sprintBackLogId),
             PATH(Int32, sprintId),
             REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        m_sprintService.deleteSprintBacklog(int(request->getBundleData<Int64>("userId")), sprintId, sprintBackLogId);
        return createResponse(Status::CODE_200, nullptr);
    }

    ENDPOINT_INFO(getSprintBacklogs)
    {
        info->summary = "Create new User";

        info->addConsumes<Object<UserDto>>("application/json");

        info->addResponse<Object<ReturnUserDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("GET", "sprints/backlog", getSprintBacklogs,
             QUERY(Int32, sprintId),
             REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        return createDtoResponse(Status::CODE_200, m_sprintService.getSprintBacklogs(int(request->getBundleData<Int64>("userId")), sprintId));
    }

    ENDPOINT_INFO(updateSprintBacklog)
    {
        info->summary = "Create new User";

        info->addConsumes<Object<UserDto>>("application/json");

        info->addResponse<Object<ReturnUserDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("PUT", "sprints/backlog", updateSprintBacklog,
             BODY_DTO(Object<UpdateSprintBacklogDto>, dto),
             REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        m_sprintService.updateSprintBacklog(int(request->getBundleData<Int64>("userId")), dto);
        return createResponse(Status::CODE_200, nullptr);
    }
};

#include OATPP_CODEGEN_END(ApiController) //<- End Codegen