//
// Created by Gao,Pengfei(FFTD) on 2018/6/6.
//

#ifndef CROW_ALL_DOMAIN_H
#define CROW_ALL_DOMAIN_H

#include <sstream>
#include "crow.h"
#include "crow.h"
#include "helper.h"
#include "crow/json.h"
#include "DomainException.h"
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>
#include <libvirt/libvirt-qemu.h>

class Domain {
public:
    static crow::json::wvalue getHostname(const crow::request& );
    static crow::json::wvalue getHostInfo(const crow::request& );
    static crow::json::wvalue getNodeInfo(const crow::request& );
    static crow::json::wvalue getDomainInfoById(const crow::request& ,int );
    static crow::json::wvalue getDomainInfoByUuidOrName(const crow::request& ,std::string );

    static crow::json::wvalue listDomain(const crow::request& );
    static crow::json::wvalue listAllDomain(const crow::request& );
    static crow::json::wvalue createDomain(const crow::request& );
    static virConnectPtr get_virconnetctptr(const char *);
};




class DimainLogHandler : public crow::ILogHandler
{
public:
    void log(std::string /*message*/, crow::LogLevel /*level*/) override
    {
        //            cerr << "DimainLogHandler -> " << message;
    }
};


struct DimainMiddleware
{
    std::string message;

    DimainMiddleware()
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

    void before_handle(crow::request &/*req*/, crow::response &/*res*/, context & /*ctx*/)
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
        CROW_LOG_CRITICAL << dump(list);
    }
};


#endif //CROW_ALL_DOMAIN_H
