#pragma once

#ifndef DATABASE_MIGRATIONS
#define DATABASE_MIGRATIONS "sql"
#endif

#include "oatpp-postgresql/orm.hpp"

#include "oatpp/base/Log.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient) //<- Begin Codegen

/**
 * UserDb client definitions.
 */
class MyDB : public oatpp::orm::DbClient
{
public:
    MyDB(const std::shared_ptr<oatpp::orm::Executor> &executor)
        : oatpp::orm::DbClient(executor)
    {

        oatpp::orm::SchemaMigration migration(executor);
        migration.addFile(1 /* start from version 1 */, DATABASE_MIGRATIONS "/001_init.sql");
        migration.addFile(2, DATABASE_MIGRATIONS "/002.sql");
        migration.addFile(3, DATABASE_MIGRATIONS "/003_project/project.sql");
        migration.addFile(4, DATABASE_MIGRATIONS "/004_notification.sql");
        migration.addFile(5, DATABASE_MIGRATIONS "/005_project_invitation.sql");
        migration.addFile(6, DATABASE_MIGRATIONS "/006_product_backlog.sql");
        migration.addFile(7, DATABASE_MIGRATIONS "/007_sprint_functions.sql");
        migration.addFile(8, DATABASE_MIGRATIONS "/008_sprint_backlog_functions.sql");
        migration.addFile(9, DATABASE_MIGRATIONS "/009_sprint_backlog_triggers.sql");
        migration.addFile(10, DATABASE_MIGRATIONS "/010_backlog_query.sql");
        // TODO - Add more migrations here.
        migration.migrate(); // <-- run migrations. This guy will throw on error.

        auto version = executor->getSchemaVersion();
        OATPP_LOGd("UserDb", "Migration - OK. Version={}.", version);
    }
};

#include OATPP_CODEGEN_END(DbClient) //<- End Codegen