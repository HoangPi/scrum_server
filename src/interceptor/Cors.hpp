#include "oatpp/web/server/interceptor/ResponseInterceptor.hpp"

class MyCorsInterceptor : public oatpp::web::server::interceptor::ResponseInterceptor {
public:
  std::shared_ptr<OutgoingResponse> intercept(const std::shared_ptr<IncomingRequest>& request,
                                            const std::shared_ptr<OutgoingResponse>& response) override {
    
    // Set your standard CORS headers
    response->putHeader("Access-Control-Allow-Origin", getenv("ALLOWED_ORIGINS"));
    response->putHeader("Access-Control-Allow-Methods", getenv("ALLOWED_METHODS"));
    response->putHeader("Access-Control-Allow-Headers", getenv("ALLOWED_HEADERS"));
    response->putHeader("Access-Control-Max-Age", getenv("MAX_AGE"));
    
    // THIS IS THE MISSING PIECE:
    response->putHeader("Access-Control-Allow-Credentials", "true");
    
    return response;
  }
};