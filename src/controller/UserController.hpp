
#ifndef UserController_hpp
#define UserController_hpp

#include "service/UserService.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/web/mime/ContentMappers.hpp"
#include "oatpp/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

/**
 * User REST controller.
 */
class UserController : public oatpp::web::server::api::ApiController
{
public:
  UserController(OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
      : oatpp::web::server::api::ApiController(apiContentMappers)
  {
  }

private:
  UserService m_userService; // Create user service.
public:
  static std::shared_ptr<UserController> createShared(
      OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers) // Inject ContentMappers
  )
  {
    return std::make_shared<UserController>(apiContentMappers);
  }

  ENDPOINT_INFO(createUser)
  {
    info->summary = "Create new User";

    info->addConsumes<Object<UserDto>>("application/json");

    info->addResponse<Object<ReturnUserDto>>(Status::CODE_200, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
  }
  ENDPOINT("POST", "users", createUser,
           BODY_DTO(Object<UserDto>, userDto))
  {
    return createDtoResponse(Status::CODE_200, m_userService.createUser(userDto));
  }

  ENDPOINT_INFO(putUser)
  {
    info->summary = "Update User by userId";

    info->addConsumes<Object<UserDto>>("application/json");

    info->addResponse<Object<UserDto>>(Status::CODE_200, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");

    info->pathParams["userId"].description = "User Identifier";
  }
  ENDPOINT("PUT", "users", putUser,
           BODY_DTO(Object<UserDto>, userDto),
           REQUEST(std::shared_ptr<IncomingRequest>, request))
  {
    userDto->id = int(request->getBundleData<Int64>("userId"));
    m_userService.updateUser(userDto);
    return createResponse(Status::CODE_200, nullptr);
  }

  ENDPOINT_INFO(changePassword)
  {
    info->summary = "Update User by userId";

    info->addConsumes<Object<UserDto>>("application/json");

    info->addResponse<Object<UserDto>>(Status::CODE_200, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");

    info->pathParams["userId"].description = "User Identifier";
  }
  ENDPOINT("PUT", "users/change_password", changePassword,
           BODY_DTO(Object<ChangePasswordDto>, dto),
           REQUEST(std::shared_ptr<IncomingRequest>, request))
  {
    m_userService.changePassword(request->getBundleData<Int64>("userId"), dto->oldPassword, dto->newPassword);
    return createResponse(Status::CODE_200, nullptr);
  }

  ENDPOINT_INFO(getUserById)
  {
    info->summary = "Get one User by userId";

    info->addResponse<Object<ReturnUserDto>>(Status::CODE_200, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");

    info->pathParams["userId"].description = "User Identifier";
  }
  ENDPOINT("GET", "users", getUserById,
           REQUEST(std::shared_ptr<IncomingRequest>, request))
  {
    return createDtoResponse(Status::CODE_200, m_userService.getUserById(request->getBundleData<oatpp::Int64>("userId")));
  }

  ENDPOINT_INFO(verifyUser)
  {
    info->summary = "find user by username and password";

    info->addConsumes<Object<LoginUserDto>>("application/json");
    info->addResponse<Object<AuthDto>>(Status::CODE_200, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
  }
  ENDPOINT("POST", "users/login", verifyUser, BODY_DTO(Object<LoginUserDto>, dto))
  {
    return createDtoResponse(Status::CODE_200, m_userService.verifyUser(dto));
  }

  ENDPOINT_INFO(refreshToken)
  {
    info->summary = "find user by username and password";

    info->addConsumes<Object<LoginUserDto>>("application/json");
    info->addResponse<Object<AuthDto>>(Status::CODE_200, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
  }
  ENDPOINT("POST", "users/refresh", refreshToken, BODY_DTO(Object<AuthDto>, dto))
  {
    return createDtoResponse(Status::CODE_200, m_userService.refreshToken(dto->refresh));
  }

  // ENDPOINT_INFO(getUsers) {
  //   info->summary = "get all stored users";

  //   info->addResponse<oatpp::Object<UsersPageDto>>(Status::CODE_200, "application/json");
  //   info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
  // }
  // ENDPOINT("GET", "users/offset/{offset}/limit/{limit}", getUsers,
  //          PATH(UInt32, offset),
  //          PATH(UInt32, limit))
  // {
  //   return createDtoResponse(Status::CODE_200, m_userService.getAllUsers(offset, limit));
  // }

  ENDPOINT_INFO(deleteUser)
  {
    info->summary = "Delete User by userId";

    info->addResponse<Object<StatusDto>>(Status::CODE_200, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");

    info->pathParams["userId"].description = "User Identifier";
  }
  ENDPOINT("DELETE", "users/{userId}", deleteUser,
           PATH(Int32, userId))
  {
    return createDtoResponse(Status::CODE_200, m_userService.deleteUserById(userId));
  }

  ENDPOINT_INFO(getNotifications)
  {
    info->summary = "Get one User by userId";

    info->addResponse<Object<ReturnUserDto>>(Status::CODE_200, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");

    info->pathParams["userId"].description = "User Identifier";
  }
  ENDPOINT("GET", "notifications", getNotifications,
           QUERY(oatpp::Int32, offset),
           REQUEST(std::shared_ptr<IncomingRequest>, request))
  {
    return createDtoResponse(
        Status::CODE_200,
        m_userService.getNotifications(int(request->getBundleData<oatpp::Int64>("userId")), offset));
  }

  ENDPOINT_INFO(handleReadNotification)
  {
    info->summary = "Get one User by userId";

    info->addResponse<Object<ReturnUserDto>>(Status::CODE_200, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");

    info->pathParams["userId"].description = "User Identifier";
  }
  ENDPOINT("POST", "notifications", handleReadNotification,
           QUERY(oatpp::Int32, id),
           QUERY(oatpp::Boolean, willDelete),
           REQUEST(std::shared_ptr<IncomingRequest>, request))
  {
    m_userService.handleReadNotification(int(request->getBundleData<oatpp::Int64>("userId")), id, willDelete);
    return createResponse(
        Status::CODE_200,
        nullptr);
  }
  ENDPOINT_INFO(getUsersByEmail)
  {
    info->summary = "Get one User by userId";

    info->addResponse<Object<ReturnUserDto>>(Status::CODE_200, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");

    info->pathParams["userId"].description = "User Identifier";
  }
  // TODO There is a bug where sending get request with body the second time crash the system
  ENDPOINT("GET", "user", getUsersByEmail,
           QUERY(oatpp::String, email),
           REQUEST(std::shared_ptr<IncomingRequest>, request))
  {
    return createDtoResponse(
        Status::CODE_200,
        m_userService.getUsersByEmail(email));
  }
};

#include OATPP_CODEGEN_END(ApiController) //<- End Codegen

#endif /* UserController_hpp */
