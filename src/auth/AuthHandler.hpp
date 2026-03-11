#pragma once

#include "JWT.hpp"

class AuthHandler : public oatpp::web::server::handler::BearerAuthorizationHandler
{
private:
  std::shared_ptr<JWT> m_jwt;

public:
  AuthHandler(const std::shared_ptr<JWT> &jwt);

  std::shared_ptr<AuthorizationObject> authorize(const oatpp::String &token) override;
  virtual std::shared_ptr<oatpp::web::server::handler::AuthorizationObject> handleAuthorization(const oatpp::String &header) override;
};
