#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class CreateSprintDto : public oatpp::DTO
{

    DTO_INIT(CreateSprintDto, DTO)

    DTO_FIELD(Int32, projectId, "project_id");
    DTO_FIELD(String, name, "name");
    DTO_FIELD(String, goal, "goal");
    DTO_FIELD(String, startDate, "start_date");
};

class SprintDto : public oatpp::DTO
{

    DTO_INIT(SprintDto, DTO)

    DTO_FIELD(Int32, id, "id");
    DTO_FIELD(Int32, projectId, "project_id");
    DTO_FIELD(String, name, "name");
    DTO_FIELD(String, goal, "goal");
    DTO_FIELD(String, startDate, "start_date");
    DTO_FIELD(String, endDate, "end_date");
    DTO_FIELD(String, status, "status");
};

#include OATPP_CODEGEN_END(DTO)

#include OATPP_CODEGEN_END(DTO)