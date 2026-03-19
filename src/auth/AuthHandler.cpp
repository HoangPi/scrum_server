#include "AuthHandler.hpp"

AuthHandler::AuthHandler(const std::shared_ptr<JWT> &jwt)
    : oatpp::web::server::handler::BearerAuthorizationHandler("API" /* Realm */), m_jwt(jwt)
{
}

std::shared_ptr<AuthHandler::AuthorizationObject> AuthHandler::authorize(const oatpp::String &token)
{
  try
  {
    auto t = token->substr(7); // remove the "bearer"
    return m_jwt->readAndVerifyToken(t);
  }
  catch (...)
  {
    OATPP_ASSERT_HTTP(false, oatpp::web::protocol::http::Status::CODE_401, "Token has expired");
  }
}

std::shared_ptr<oatpp::web::server::handler::AuthorizationObject> AuthHandler::handleAuthorization(const oatpp::String &header)
{
  // auto aim = oatpp::web::server::handler::BearerAuthorizationHandler::handleAuthorization(header);
  return std::shared_ptr<oatpp::web::server::handler::AuthorizationObject>();
}
