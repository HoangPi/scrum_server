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

#include OATPP_CODEGEN_END(DTO)