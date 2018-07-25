//
// Created by Gao,Pengfei(FFTD) on 2018/6/18.
//

#ifndef CROW_ALL_MIDDLEWARE_H
#define CROW_ALL_MIDDLEWARE_H

#include "helper.h"

struct middleware
{
    std::string message;

    middleware()
    {
        message = "foo";
    }

    void setMessage(std::string newMsg)
    {
        message = newMsg;
    }
    struct context
    {
    };

    void before_handle(crow::request &/*req*/, crow::response &res, context &/*ctx*/)
    {
        CROW_LOG_DEBUG << " - MESSAGE: " << message;
    }

    void after_handle(crow::request &req, crow::response &res, context & /*ctx*/)
    {
        crow::json::wvalue list;
        std::string content;
        list["request"]["method"]           = crow::method_name(req.method);
        list["request"]["raw_url"]          = req.raw_url;
        list["request"]["url"]              = req.url;
        list["request"]["body"]             = req.body;
        list["request"]["cookie"]           = req.get_header_value("Cookie");
        list["request"]["referer"]          = req.get_header_value("Referer");
        list["request"]["User-Agent"]       = req.get_header_value("User-Agent");
        list["request"]["Host"]             = req.get_header_value("Host");
        list["response"]["body"]            = res.body;
        list["response"]["json_value"]      = dump(res.json_value);
        list["response"]["code"]            = res.code;
        res.add_header("Access-Control-Allow-Origin","*");
        res.add_header("Access-Control-Allow-Methods","GET,POST,PUT,DELETE,OPTION,HEAD,CONNECT,TRACE");
        CROW_LOG_CRITICAL << dump(list);
    }
};

#endif //CROW_ALL_MIDDLEWARE_H
