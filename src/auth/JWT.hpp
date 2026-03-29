#pragma once

#include "oatpp/web/server/handler/AuthorizationHandler.hpp"
#include "oatpp/Types.hpp"

#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/kazuho-picojson/traits.h>

extern const char *secretKey;

class JWT
{
public:
    struct Payload : public oatpp::web::server::handler::AuthorizationObject
    {

        oatpp::Int64 userId;
        oatpp::String username;
        oatpp::String role;
        oatpp::String email;
    };

private:
    oatpp::String m_secret;
    oatpp::String m_secret_refresh;
    oatpp::String m_issuer;
    jwt::verifier<jwt::default_clock, jwt::traits::kazuho_picojson> m_verifier;
    jwt::verifier<jwt::default_clock, jwt::traits::kazuho_picojson> m_verifier_refresh;

public:
    JWT(const oatpp::String &secret,
        const oatpp::String &secret_refresh,
        const oatpp::String &issuer);

    oatpp::String createToken(const std::shared_ptr<Payload> &payload);

    std::shared_ptr<Payload> readAndVerifyToken(const oatpp::String &token);
    std::shared_ptr<Payload> readAndVerifyRefreshToken(const oatpp::String &token);
};