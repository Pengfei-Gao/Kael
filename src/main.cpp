#include <sstream>
#include "crow.h"
#include "domain/domain.h"
#include "helper.h"
#include "router.h"

int main()
{
    crow::App<DimainMiddleware> app;

    app.get_middleware<DimainMiddleware>().setMessage("hello,KAEL");

    KAEL_ROUTE(app,"/api/domain/list",Domain::listDomain(req),GET,...)
    KAEL_ROUTE(app,"/api/node/hostname",Domain::getHostname(req),GET,...)
    KAEL_ROUTE(app,"/api/domain/create",Domain::createDomain(req),POST,...)
    KAEL_ROUTE(app,"/api/node/info",Domain::getNodeInfo(req),GET,...)
    KAEL_ROUTE(app,"/api/host/info",Domain::getHostInfo(req),GET,...)


    KAEL_ROUTE_REST(app,"/api/domain/<int>",Domain::getDomainInfoById(req,domain_id),int domain_id,...)
    KAEL_ROUTE_REST(app,"/api/domain/<string>",Domain::getDomainInfoByUuidOrName(req,uuidOrname),std::string uuidOrname,...)
    // enables all log
    app.loglevel(crow::LogLevel::INFO);
    //crow::logger::setHandler(std::make_shared<DimainLogHandler>());
    app.port(8000).multithreaded().run();
}
