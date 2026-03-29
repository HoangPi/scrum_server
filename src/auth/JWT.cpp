#include "JWT.hpp"

JWT::JWT(const oatpp::String &secret, const oatpp::String &secret_refresh, const oatpp::String &issuer) : m_secret_refresh(secret_refresh),
                                                                                                          m_secret(secret), m_issuer(issuer),
                                                                                                          m_verifier(
                                                                                                              jwt::verify()
                                                                                                                  .allow_algorithm(jwt::algorithm::hs384(secret))
                                                                                                                  .expires_at_leeway(10 * 60)
                                                                                                                  .with_issuer(issuer)),
                                                                                                          m_verifier_refresh(
                                                                                                              jwt::verify()
                                                                                                                  .allow_algorithm(jwt::algorithm::hs384(m_secret_refresh))
                                                                                                                  .expires_at_leeway(10 * 60)
                                                                                                                  .with_issuer(issuer)),
                                                                                                          m_token_expire(getenv("JWT_EXPIRE")
                                                                                                                             ? atoi(getenv("JWT_EXPIRE"))
                                                                                                                             : 3600),
                                                                                                          m_refresh_expire(getenv("JWT_REFRESH_EXPIRE")
                                                                                                                               ? atoi(getenv("JWT_REFRESH_EXPIRE"))
                                                                                                                               : 3600)
{
}

oatpp::String JWT::createToken(const std::shared_ptr<Payload> &payload, bool isRefresh)
{
    const int defaultExpire = 36000;
    if (isRefresh)
    {
        auto refresh = jwt::create()
                           .set_issuer(m_issuer)
                           .set_type("JWS")
                           .set_payload_claim("userId", picojson::value((payload->userId)))
                           .set_payload_claim("username", jwt::claim(payload->username))
                           .set_payload_claim("email", jwt::claim(payload->email))
                           .set_payload_claim("role", jwt::claim(payload->role))
                           .set_expires_in(std::chrono::seconds(m_refresh_expire.getValue(defaultExpire)))
                           .sign(jwt::algorithm::hs384(m_secret_refresh));
        return refresh;
    }
    auto token = jwt::create()
                     .set_issuer(m_issuer)
                     .set_type("JWS")
                     .set_payload_claim("userId", picojson::value((payload->userId)))
                     .set_payload_claim("username", jwt::claim(payload->username))
                     .set_payload_claim("email", jwt::claim(payload->email))
                     .set_payload_claim("role", jwt::claim(payload->role))
                     .set_expires_in(std::chrono::seconds(m_token_expire.getValue(defaultExpire)))
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

std::shared_ptr<JWT::Payload> JWT::readAndVerifyRefreshToken(const oatpp::String &token)
{
    auto decoded = jwt::decode(token);
    m_verifier_refresh.verify(decoded);

    auto payload = std::make_shared<Payload>();
    payload->userId = decoded.get_payload_claim(jwt::traits::kazuho_picojson::string_type("userId")).as_integer();
    payload->email = decoded.get_payload_claim(jwt::traits::kazuho_picojson::string_type("email")).as_string();
    payload->role = decoded.get_payload_claim(jwt::traits::kazuho_picojson::string_type("role")).as_string();
    payload->username = decoded.get_payload_claim(jwt::traits::kazuho_picojson::string_type("username")).as_string();

    return payload;
}
