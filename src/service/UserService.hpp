#pragma once

#include "auth/AuthHandler.hpp"
#include "dto/AuthDto.hpp"

#include "db/UserDb.hpp"
#include "dto/PageDto.hpp"
#include "dto/StatusDto.hpp"

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/macro/component.hpp"

class UserService
{
private:
    typedef oatpp::web::protocol::http::Status Status;

private:
    OATPP_COMPONENT(std::shared_ptr<UserDb>, m_database); // Inject database component
    OATPP_COMPONENT(std::shared_ptr<JWT>, m_jwt); // Inject database component
public:
    oatpp::Object<ReturnUserDto> createUser(const oatpp::Object<UserDto> &dto);
    oatpp::Object<AuthDto> verifyUser(const oatpp::Object<LoginUserDto> &dto);
    oatpp::Object<UserDto> getUserByUsername(const oatpp::String &username);
    void updateUser(const oatpp::Object<UserDto> &dto);
    void changePassword(const oatpp::Int64 &userId, const oatpp::String &oldPassword, const oatpp::String &newPassword);
    oatpp::Object<ReturnUserDto> getUserById(const oatpp::Int64 &id, const oatpp::provider::ResourceHandle<oatpp::orm::Connection> &connection = nullptr);
    oatpp::Object<PageDto<oatpp::Object<UserDto>>> getAllUsers(const oatpp::UInt32 &offset, const oatpp::UInt32 &limit);
    oatpp::Object<StatusDto> deleteUserById(const oatpp::Int32 &id);
    oatpp::Vector<oatpp::Object<NotificationDto>> getNotifications(const oatpp::Int32 &userId, const oatpp::Int32 &offset);
};