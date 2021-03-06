#include <sstream>
#include "crow.h"
#include "domain/domain.h"
#include "helper.h"
#include "middleware.h"
#include "router.h"

int main()
{
    crow::App<middleware> app;

    app.get_middleware<middleware>().setMessage("hello,KAEL");

    KEAL_ROUTE_T(app,"/api/domain/list",Domain::listDomain(req),GET,...)
    KEAL_ROUTE_T(app,"/api/node/hostname",Domain::getHostname(req),GET,...)
    KEAL_ROUTE_T(app,"/api/domain/create",Domain::createDomain(req),POST,...)
    KEAL_ROUTE_T(app,"/api/node/info",Domain::getNodeInfo(req),GET,...)
    KEAL_ROUTE_T(app,"/api/host/info",Domain::getHostInfo(req),GET,...)

    KEAL_ROUTE_T(app,"/api/domain/<string>/shutdown",Domain::shutdownByUuidOrname(req,uuidOrname),PUT,std::string uuidOrname,...)
    KEAL_ROUTE_T(app,"/api/domain/<int>/shutdown",Domain::shutdownById(req,id),PUT,int id,...)

    KEAL_ROUTE_T(app,"/api/domain/<string>/destroy",Domain::destroyByUuidOrname(req,uuidOrname),PUT,std::string uuidOrname,...)
    KEAL_ROUTE_T(app,"/api/domain/<int>/destroy",Domain::destroyById(req,id),PUT,int id,...)

    KEAL_ROUTE_T(app,"/api/domain/<string>/start",Domain::startByUuidOrname(req,uuidOrname),PUT,std::string uuidOrname,...)
    KEAL_ROUTE_T(app,"/api/domain/<int>/start",Domain::startById(req,id),PUT,int id,...)


    KEAL_ROUTE_T(app,"/api/domain/<string>/save",Domain::saveByUuidOrname(req,uuidOrname),POST,std::string uuidOrname,...)
    KEAL_ROUTE_T(app,"/api/domain/<int>/save",Domain::saveById(req,id),POST,int id,...)

    KEAL_ROUTE_T(app,"/api/domain/restore",Domain::restore(req),POST,...)

    KAEL_ROUTE_ANY(app,"/api/domain/<int>",Domain::getDomainInfoById(req,domain_id),int domain_id,...)
    KAEL_ROUTE_ANY(app,"/api/domain/<string>",Domain::getDomainInfoByUuidOrName(req,uuidOrname),std::string uuidOrname,...)


    // enables all log
    app.loglevel(crow::LogLevel::INFO);
    //crow::logger::setHandler(std::make_shared<DimainLogHandler>());
    app.port(8000).multithreaded().run();
}
