#pragma once

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class AuthDto : public oatpp::DTO
{
  DTO_INIT(AuthDto, DTO)

  DTO_FIELD(String, token, "token");
};

#include OATPP_CODEGEN_END(DTO)