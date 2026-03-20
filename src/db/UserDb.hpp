
#ifndef CRUD_USERDB_HPP
#define CRUD_USERDB_HPP

#ifndef DATABASE_MIGRATIONS
#define DATABASE_MIGRATIONS "sql"
#endif

#include "dto/UserDto.hpp"
#include "oatpp-postgresql/orm.hpp"

#include "oatpp/base/Log.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient) //<- Begin Codegen

/**
 * UserDb client definitions.
 */
class UserDb : public oatpp::orm::DbClient
{
public:
      UserDb(const std::shared_ptr<oatpp::orm::Executor> &executor)
          : oatpp::orm::DbClient(executor)
      {
      }

      QUERY(createUser,
            "INSERT INTO AppUser"
            "(username, email, password, name) VALUES "
            "(:user.username, :user.email, :user.password, :user.name)"
            "RETURNING (id);",
            PREPARE(true), // user prepared statement!
            PARAM(oatpp::Object<UserDto>, user))

      QUERY(updateUser,
            "UPDATE AppUser "
            "SET "
            " username=:user.username, "
            " email=:user.email, "
            " password=:user.password, "
            " role=:user.role "
            "WHERE "
            " id=:user.id;",
            PARAM(oatpp::Object<UserDto>, user))

      QUERY(getUserById,
            "select * from get_user_by_id(:id)",
            PREPARE(true), //<-- user prepared statement!
            PARAM(oatpp::Int64, id))

      QUERY(getUserByUsername,
            "select * from get_user_by_username(:username)",
            PREPARE(true),
            PARAM(oatpp::String, username))

      QUERY(getAllUsers,
            "SELECT * FROM AppUser LIMIT :limit OFFSET :offset;",
            PARAM(oatpp::UInt32, offset),
            PARAM(oatpp::UInt32, limit))

      QUERY(deleteUserById,
            "DELETE FROM AppUser WHERE id=:id;",
            PARAM(oatpp::Int32, id))

      QUERY(getNotifications,
            "SELECT id, priority, message, path, has_read FROM Notification "
            "WHERE owner = :userId "
            "ORDER BY has_read ASC, created_at DESC "
            " LIMIT 20 OFFSET 20 * :offset;",
            PARAM(oatpp::Int32, userId),
            PARAM(oatpp::Int32, offset));

      QUERY(updateUserInfo,
            "UPDATE AppUser SET name = :dto.name, email = :dto.email WHERE id = :dto.id",
            PREPARE(true),
            PARAM(oatpp::Object<UserDto>, dto));

      QUERY(updateUserPassword,
            "UPDATE AppUser SET password = :password WHERE id = :userId",
            PREPARE(true),
            PARAM(oatpp::Int64, userId),
            PARAM(oatpp::String, password));
};

#include OATPP_CODEGEN_END(DbClient) //<- End Codegen

#endif // CRUD_USERDB_HPP
