#pragma once

#include "service/UserService.hpp"
#include "service/project/ProjectService.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/web/mime/ContentMappers.hpp"
#include "oatpp/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

/**
 * User REST controller.
 */
class ProjectController : public oatpp::web::server::api::ApiController
{
public:
    ProjectController(OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
        : oatpp::web::server::api::ApiController(apiContentMappers)
    {
    }

private:
    UserService m_userService; // Create user service.
    ProjectService m_projectService;
public:
    static std::shared_ptr<ProjectController> createShared(
        OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers) // Inject ContentMappers
    )
    {
        return std::make_shared<ProjectController>(apiContentMappers);
    }

    ENDPOINT_INFO(createProject)
    {
        info->summary = "Create new User";

        info->addConsumes<Object<UserDto>>("application/json");

        info->addResponse<Object<ReturnUserDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("POST", "project", createProject,
             BODY_DTO(Object<ProjectDto>, dto),
             REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        dto->projectOwner = request->getBundleData<Int64>("userId");
        this->m_projectService.createProject(dto);
        return createResponse(Status::CODE_200, nullptr);
    }
};

#include OATPP_CODEGEN_END(ApiController) //<- End Codegen
