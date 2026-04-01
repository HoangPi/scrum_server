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

class InviteManyDto : public oatpp::DTO
{
    DTO_INIT(InviteManyDto, DTO)

    DTO_FIELD(Int32, projectId, "project_id");
    DTO_FIELD(Vector<Int32>, invitees, "invitees");
};

class MemberInfo : public oatpp::DTO
{
    DTO_INIT(MemberInfo, DTO)

    DTO_FIELD(Int32, id, "id");
    DTO_FIELD(String, name, "name");
    DTO_FIELD(String, email, "email");
};

class MemberInfoWithRoleDto : public oatpp::DTO
{
    DTO_INIT(MemberInfoWithRoleDto, DTO)

    DTO_FIELD(Int32, id, "id");
    DTO_FIELD(String, name, "name");
    DTO_FIELD(String, email, "email");
    DTO_FIELD(String, role, "role");
};

class MembersInProjectsDto : public oatpp::DTO
{
    DTO_INIT(MembersInProjectsDto, DTO)

    DTO_FIELD(Vector<Object<MemberInfoWithRoleDto>>, PO, "PO");
    DTO_FIELD(Vector<Object<MemberInfoWithRoleDto>>, SM, "SM");
    DTO_FIELD(Vector<Object<MemberInfoWithRoleDto>>, EM, "EM");
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

class ProjectOverviewDto : public oatpp::DTO
{
    DTO_INIT(ProjectOverviewDto, DTO)

    DTO_FIELD(Int32, projectId, "project_id") = nullptr;
    DTO_FIELD(Int32, totalProductBacklogs, "total_product_backlogs");
    DTO_FIELD(Int32, finishedProductBacklogs, "finished_product_backlogs");
    DTO_FIELD(Int32, totalStoryPoints, "total_story_points");
    DTO_FIELD(Int32, finishedStoryPoints, "finished_story_points");
    DTO_FIELD(Int32, totalUserStories, "total_user_stories");
    DTO_FIELD(Int32, finishedUserStories, "finished_user_stories");
    DTO_FIELD(Int32, failedUserStories, "failed_user_stories");
};

#include OATPP_CODEGEN_END(DTO)