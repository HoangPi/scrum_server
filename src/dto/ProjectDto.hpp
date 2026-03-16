#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class ProjectDto : public oatpp::DTO
{

    DTO_INIT(ProjectDto, DTO)

    DTO_FIELD(Int32, id, "id");
    DTO_FIELD(Int32, projectOwner, "project_owner");
    DTO_FIELD(String, name, "name");
    DTO_FIELD(String, description, "description");

    DTO_FIELD(Int32, sprintLength, "sprint_length");
    DTO_FIELD(Int32, sprintStartDay, "sprint_start_day");
    DTO_FIELD(Int32, workDayHour, "work_day_hour");
    DTO_FIELD(Int32, defaultDayOff, "default_day_off");
    DTO_FIELD(Vector<String>, dayOff, "day_off") = nullptr;

    DTO_FIELD(String, status, "status");
};

class InviteDto : public oatpp::DTO
{

    DTO_INIT(InviteDto, DTO)

    DTO_FIELD(Int32, id, "id");
    DTO_FIELD(Int32, invitee, "invitee");
    DTO_FIELD(Int32, projectId, "project_id");
    DTO_FIELD(String, status, "status") = "created";
};

class MemberInfo : public oatpp::DTO
{
    DTO_INIT(MemberInfo, DTO)

    DTO_FIELD(Int32, id, "id");
    DTO_FIELD(String, name, "name");
    DTO_FIELD(String, email, "email");
};

class MemberDto : public oatpp::DTO
{

    DTO_INIT(MemberDto, DTO)

    DTO_FIELD(Int32, userId, "user_id");
    DTO_FIELD(Int32, projectId, "project_id");
    DTO_FIELD(String, role, "role");
};

class ProductBacklogDto : public oatpp::DTO
{

    DTO_INIT(ProductBacklogDto, DTO)

    DTO_FIELD(Int32, id, "id");
    DTO_FIELD(Int32, projectId, "project_id");
    DTO_FIELD(String, name, "name");
    DTO_FIELD(String, acceptanceCriteria, "acceptance_criteria");
    DTO_FIELD(Int32, priority, "priority");
    DTO_FIELD(String, status, "status") = "created";
    DTO_FIELD(Int32, storyPoint, "story_point");
};

#include OATPP_CODEGEN_END(DTO)