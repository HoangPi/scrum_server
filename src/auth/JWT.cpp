#include "JWT.hpp"

JWT::JWT(const oatpp::String &secret, const oatpp::String &issuer) : m_secret(secret), m_issuer(issuer), m_verifier(jwt::verify()
                                                                                                                        .allow_algorithm(jwt::algorithm::hs384(secret))
                                                                                                                        .expires_at_leeway(3600)
                                                                                                                        .with_issuer(issuer))
{
}

oatpp::String JWT::createToken(const std::shared_ptr<Payload> &payload)
{
    auto token = jwt::create()
                     .set_issuer(m_issuer)
                     .set_type("JWS")
                     .set_payload_claim("userId", picojson::value((payload->userId)))
                     .set_payload_claim("username", jwt::claim(payload->username))
                     .set_payload_claim("email", jwt::claim(payload->email))
                     .set_payload_claim("role", jwt::claim(payload->role))
                     .set_expires_in(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::hours(2)))
                     .sign(jwt::algorithm::hs384(m_secret));
    return token;
}

std::shared_ptr<JWT::Payload> JWT::readAndVerifyToken(const oatpp::String &token)
{
    auto decoded = jwt::decode(token);
    m_verifier.verify(decoded);

    auto payload = std::make_shared<Payload>();
    payload->userId = decoded.get_payload_claim(jwt::traits::kazuho_picojson::string_type("userId")).as_integer();
    payload->email = decoded.get_payload_claim(jwt::traits::kazuho_picojson::string_type("email")).as_string();
    payload->role = decoded.get_payload_claim(jwt::traits::kazuho_picojson::string_type("role")).as_string();
    payload->username = decoded.get_payload_claim(jwt::traits::kazuho_picojson::string_type("username")).as_string();

    return payload;
}
