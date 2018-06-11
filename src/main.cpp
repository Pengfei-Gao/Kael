#include <sstream>
#include "crow.h"
#include "domain/domain.h"
#include "helper.h"
#include "router.h"

int main()
{
    crow::App<DimainMiddleware> app;

    app.get_middleware<DimainMiddleware>().setMessage("hello,KAEL");

    KAEL_ROUTE(app,"/api/domain/list",Domain::list_domain(req),GET,...)
    KAEL_ROUTE(app,"/api/domain/hostname",Domain::get_hostname(req),GET,...)
    KAEL_ROUTE(app,"/api/domain/create",Domain::create_domain(req),POST,...)

    KAEL_ROUTE(app,"/api/host/info",Domain::get_nodeinfo(req),GET,...)

    // enables all log
    app.loglevel(crow::LogLevel::INFO);
    //crow::logger::setHandler(std::make_shared<DimainLogHandler>());
    app.port(8000).multithreaded().run();
}
