#include "oatpp/web/server/interceptor/ResponseInterceptor.hpp"

class MyCorsInterceptor : public oatpp::web::server::interceptor::ResponseInterceptor
{
private:
  oatpp::Vector<oatpp::String> m_allowedOrigins;
  static oatpp::Vector<oatpp::String> split(const std::string &s, char delimiter)
  {
    oatpp::Vector<oatpp::String> tokens = oatpp::Vector<oatpp::String>::createShared();
    std::istringstream iss(s);
    std::string token;
    while (std::getline(iss, token, delimiter))
    {
      tokens->push_back(token);
    }
    return tokens;
  }

public:
  MyCorsInterceptor()
  {
    std::string defaultStr = "";
    oatpp::String origins = getenv("ALLOWED_ORIGINS");
    this->m_allowedOrigins = MyCorsInterceptor::split(origins.getValue(defaultStr), ' ');
  }
  std::shared_ptr<OutgoingResponse> intercept(const std::shared_ptr<IncomingRequest> &request,
                                              const std::shared_ptr<OutgoingResponse> &response) override
  {

    // Set your standard CORS headers
    // response->putHeader("Access-Control-Allow-Origin", getenv("ALLOWED_ORIGINS"));
    for (size_t i = 0; i < m_allowedOrigins->size(); i++)
    {
      if (request->getHeader("origin").equalsCI_ASCII(m_allowedOrigins[i]))
      {
        response->putHeader("Access-Control-Allow-Origin", m_allowedOrigins[i]);
        break;
      }
    }

    response->putHeader("Access-Control-Allow-Methods", getenv("ALLOWED_METHODS"));
    response->putHeader("Access-Control-Allow-Headers", getenv("ALLOWED_HEADERS"));
    response->putHeader("Access-Control-Max-Age", getenv("MAX_AGE"));

    response->putHeader("Access-Control-Allow-Credentials", "true");

    return response;
  }
};