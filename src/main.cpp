#include <sstream>
#include "crow.h"
#include "domain/domain.h"

int main()
{
    crow::App<DimainMiddleware> app;

    app.get_middleware<DimainMiddleware>().setMessage("hello");

    CROW_ROUTE(app, "/")
    ([]() {
        crow::json::wvalue list;
        list["message"] = "Hello, World!";
        return list;
    });

    CROW_ROUTE(app,"/api/domain/list")
    ([](const crow::request& req) {
        return  Domain::list_domain(req);
    });

    CROW_ROUTE(app,"/api/domain/hostname")
    ([](const crow::request& req) {
        return  Domain::get_hostname(req);
    });

    // enables all log
    app.loglevel(crow::LogLevel::DEBUG);

    //crow::logger::setHandler(std::make_shared<DimainLogHandler>());

    app.port(8000)
        .multithreaded()
        .run();
}
