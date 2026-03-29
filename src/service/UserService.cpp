#include "UserService.hpp"
#include "bcrypt.h"

oatpp::Object<ReturnUserDto> UserService::createUser(const oatpp::Object<UserDto> &dto)
{
    dto.get()->password = bcrypt::generateHash(dto.get()->password, 10);
    auto dbResult = m_database->createUser(dto);
    OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());

    auto result = dbResult->fetch<oatpp::Vector<oatpp::Object<UserDto>>>();
    OATPP_ASSERT_HTTP(result->size() == 1, Status::CODE_500, "Unknown error");
    return getUserById(result[0].get()->id);
}

oatpp::Object<AuthDto> UserService::verifyUser(const oatpp::Object<LoginUserDto> &dto)
{
    auto user = this->getUserByUsername(dto.get()->username);
    OATPP_ASSERT_HTTP(user.get(), Status::CODE_500, "Unkown error");

    OATPP_ASSERT_HTTP(bcrypt::validatePassword(dto.get()->password, user.get()->password), Status::CODE_404, "User not found");

    auto payload = std::make_shared<JWT::Payload>();
    payload->userId = user->id;
    payload->role = user->role;
    payload->email = user->email;
    payload->username = user->userName;

    auto auth = AuthDto::createShared();
    auth->token = m_jwt->createToken(payload);
    auth->refresh = m_jwt->createToken(payload, true);

    return auth;
}

oatpp::Object<UserDto> UserService::getUserByUsername(const oatpp::String &username)
{
    auto result = m_database->getUserByUsername(username);
    OATPP_ASSERT_HTTP(result->isSuccess(), Status::CODE_500, result->getErrorMessage());
    OATPP_ASSERT_HTTP(result->hasMoreToFetch(), Status::CODE_404, "User not found");

    auto users = result->fetch<oatpp::Vector<oatpp::Object<UserDto>>>();
    OATPP_ASSERT_HTTP(users->size() == 1, Status::CODE_500, "Unknown error");
    return users[0];
}

void UserService::updateUser(const oatpp::Object<UserDto> &dto)
{
    m_database->updateUserInfo(dto);
    return;
}

void UserService::changePassword(const oatpp::Int64 &userId, const oatpp::String &oldPassword, const oatpp::String &newPassword)
{
    // TODO Maybe have another db query to handle this
    auto user = getUserById(userId);
    auto userWithPassword = getUserByUsername(user->userName);
    OATPP_ASSERT_HTTP(bcrypt::validatePassword(oldPassword, userWithPassword->password), Status::CODE_401, "Old password is incorrect");
    const auto hashedPassword = bcrypt::generateHash(newPassword);
    m_database->updateUserPassword(userId, hashedPassword);
}

oatpp::Object<ReturnUserDto> UserService::getUserById(const oatpp::Int64 &id, const oatpp::provider::ResourceHandle<oatpp::orm::Connection> &connection)
{
    auto dbResult = m_database->getUserById(id, connection);
    OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());
    OATPP_ASSERT_HTTP(dbResult->hasMoreToFetch(), Status::CODE_404, "User not found");

    auto result = dbResult->fetch<oatpp::Vector<oatpp::Object<ReturnUserDto>>>();
    OATPP_ASSERT_HTTP(result->size() == 1, Status::CODE_500, "Unknown error");

    return result[0];
}

oatpp::Object<StatusDto> UserService::deleteUserById(const oatpp::Int32 &id)
{
    return oatpp::Object<StatusDto>();
}

oatpp::Vector<oatpp::Object<NotificationDto>> UserService::getNotifications(const oatpp::Int32 &userId, const oatpp::Int32 &offset)
{
    auto dbResult = m_database->getNotifications(userId, offset);
    OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());
    return dbResult->fetch<oatpp::Vector<oatpp::Object<NotificationDto>>>();
}

void UserService::handleReadNotification(const oatpp::Int32 &userId, const oatpp::Int32 &notificationId, const oatpp::Boolean &willDelete)
{
    bool defaultWillDelete = false;
    int defaultId = -1;
    std::shared_ptr<oatpp::orm::QueryResult> dbResult;
    if (notificationId.getValue(defaultId) != -1)
    {
        if (willDelete.getValue(defaultWillDelete))
        {
            dbResult = m_database->executeQuery("DELETE FROM notification WHERE id = :nid AND owner = :owner", {{"nid", notificationId},
                                                                                                                {"owner", userId}});
            OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());
            return;
        }
        dbResult = m_database->executeQuery("UPDATE notification SET has_read = true WHERE id = :nid AND owner = :owner", {{"nid", notificationId},
                                                                                                                           {"owner", userId}});
        OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());
        return;
    }
    if (willDelete.getValue(defaultWillDelete))
    {
        dbResult = m_database->executeQuery("DELETE FROM notification WHERE has_read = true AND owner = :owner", {{"owner", userId}});
        OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());
        return;
    }
    dbResult = m_database->executeQuery("UPDATE notification SET has_read = true WHERE owner = :owner", {{"owner", userId}});
    OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());
    return;
}

oatpp::Vector<oatpp::Object<MemberInfo>> UserService::getUsersByEmail(const oatpp::String &email)
{
    oatpp::String likeEmail = "%" + email + "%";
    auto dbResult = m_database->getUserLikeEmail(likeEmail);
    OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());
    return dbResult->fetch<oatpp::Vector<oatpp::Object<MemberInfo>>>();
}
