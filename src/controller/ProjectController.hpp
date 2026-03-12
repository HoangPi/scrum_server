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

    ENDPOINT_INFO(getProjects)
    {
        info->summary = "Create new User";

        info->addConsumes<Object<UserDto>>("application/json");

        info->addResponse<Object<ReturnUserDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("GET", "projects", getProjects,
             REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        return createDtoResponse(
            Status::CODE_200,
            m_projectService.getProjects(int(request->getBundleData<Int64>("userId"))));
    }

    ENDPOINT_INFO(getProjectById)
    {
        info->summary = "Create new User";

        info->addConsumes<Object<UserDto>>("application/json");

        info->addResponse<Object<ReturnUserDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("GET", "projects/{projectId}", getProjectById,
             PATH(Int32, projectId) /*,
              REQUEST(std::shared_ptr<IncomingRequest>, request)*/
    )
    {
        return createDtoResponse(
            Status::CODE_200,
            m_projectService.getProjectById(projectId));
    }

    ENDPOINT_INFO(createInvite)
    {
        info->summary = "Create new User";

        info->addConsumes<Object<UserDto>>("application/json");

        info->addResponse<Object<ReturnUserDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("POST", "invite/projects", createInvite,
             BODY_DTO(Object<InviteDto>, invite),
             REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        m_projectService.createInvite(invite, int(request->getBundleData<Int64>("userId")));

        return createResponse(Status::CODE_200, nullptr);
    }

    ENDPOINT_INFO(updateInvite)
    {
        info->summary = "Create new User";

        info->addConsumes<Object<UserDto>>("application/json");

        info->addResponse<Object<ReturnUserDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("POST", "invite/project", updateInvite,
             //  BODY_DTO(Object<InviteDto>, invite),
             QUERY(Int32, id),
             QUERY(oatpp::Boolean, accept),
             REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        m_projectService.updateInvite(int(request->getBundleData<Int64>("userId")), id, accept);
        return createResponse(Status::CODE_200, nullptr);
    }

    ENDPOINT_INFO(createBacklog)
    {
        info->summary = "Create new User";

        info->addConsumes<Object<UserDto>>("application/json");

        info->addResponse<Object<ReturnUserDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("POST", "project/backlog", createBacklog,
             BODY_DTO(Object<ProductBacklogDto>, backlog),
             REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        m_projectService.createProductBacklog(int(request->getBundleData<Int64>("userId")), backlog);
        return createResponse(Status::CODE_200, nullptr);
    }

    ENDPOINT_INFO(getProductBacklogs)
    {
        info->summary = "Create new User";

        info->addConsumes<Object<UserDto>>("application/json");

        info->addResponse<Object<ReturnUserDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("GET", "project/backlog", getProductBacklogs,
             QUERY(Int32, projectId),
             QUERIES(QueryParams, queryParams),
             REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        oatpp::Boolean i;
        if (queryParams.get("includeFinished"))
        {
            i = queryParams.get("includeFinished").equalsCI_ASCII("true") ? true : false;
        }
        return createDtoResponse(
            Status::CODE_200,
            m_projectService.getProductBacklogs(
                int(request->getBundleData<Int64>("userId")),
                projectId,
                std::atoi(queryParams.get("offset").get() ? queryParams.get("offset")->c_str() : "0"),
                i));
    }
};

#include OATPP_CODEGEN_END(ApiController) //<- End Codegen
