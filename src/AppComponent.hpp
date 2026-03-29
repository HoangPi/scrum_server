
#ifndef AppComponent_hpp
#define AppComponent_hpp

#include "SwaggerComponent.hpp"
#include "DatabaseComponent.hpp"
#include "auth/AuthHandler.hpp"
#include "interceptor/AuthInterceptor.hpp"

#include "ErrorHandler.hpp"

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/web/server/interceptor/AllowCorsGlobal.hpp"
#include "oatpp/web/mime/ContentMappers.hpp"

#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

#include "oatpp/json/ObjectMapper.hpp"

#include "oatpp/macro/component.hpp"
#include "interceptor/Cors.hpp"

/**
 *  Class which creates and holds Application components and registers components in oatpp::base::Environment
 *  Order of components initialization is from top to bottom
 */
class AppComponent {
public:
  
  /**
   *  Swagger component
   */
  SwaggerComponent swaggerComponent;

  /**
   * Database component
   */
  DatabaseComponent databaseComponent;

  /**
   * Create ObjectMapper component to serialize/deserialize DTOs in Controller's API
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers)([] {

    auto json = std::make_shared<oatpp::json::ObjectMapper>();
    json->serializerConfig().json.useBeautifier = true;

    auto mappers = std::make_shared<oatpp::web::mime::ContentMappers>();
    mappers->putMapper(json);

    return mappers;

  }());
  
  /**
   *  Create ConnectionProvider component which listens on the port
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([] {
    return oatpp::network::tcp::server::ConnectionProvider::createShared({"0.0.0.0", 8000, oatpp::network::Address::IP_4});
  }());
  
  /**
   *  Create Router component
   */

  OATPP_CREATE_COMPONENT(std::shared_ptr<JWT>, jwt)([]{
    return std::make_shared<JWT>(
      getenv("JWT_SECRET_KEY") ? getenv("JWT_SECRET_KEY") : "UnsafeDefaultKey", 
      getenv("JWT_SECRET_REFRESH") ? getenv("JWT_SECRET_REFRESH") : "UnsafeDefaultKey", 
      getenv("JWT_ISSUER") ? getenv("JWT_ISSUER") : "Oatpp-server");
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([] {
    return oatpp::web::server::HttpRouter::createShared();
  }());
  
  /**
   *  Create ConnectionHandler component which uses Router component to route requests
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)([] {

    OATPP_COMPONENT(std::shared_ptr<JWT>, jwt);
    
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router); // get Router component
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, contentMappers); // get ContentMappers component

    auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);
    connectionHandler->setErrorHandler(std::make_shared<ErrorHandler>(contentMappers));

    connectionHandler->addRequestInterceptor(std::make_shared<AuthInterceptor>(jwt));
    connectionHandler->addResponseInterceptor(std::make_shared<MyCorsInterceptor>());
    connectionHandler->addRequestInterceptor(std::make_shared<oatpp::web::server::interceptor::AllowOptionsGlobal>());

    connectionHandler->addResponseInterceptor(std::make_shared<oatpp::web::server::interceptor::AllowCorsGlobal>());
    return connectionHandler;

  }());

};

#endif /* AppComponent_hpp */
