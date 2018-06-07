//
// Created by Gao,Pengfei(FFTD) on 2018/6/6.
//

#ifndef CROW_ALL_DOMAIN_H
#define CROW_ALL_DOMAIN_H

#include <sstream>
#include "crow.h"
#include "helper.h"
#include "DomainException.h"
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>
#include <libvirt/libvirt-qemu.h>

class Domain {
public:
    static crow::json::wvalue get_hostname(const crow::request& );
    static crow::json::wvalue list_domain(const crow::request& );
    static crow::json::wvalue create_domain(const crow::request& );
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

    void before_handle(crow::request & /*req*/, crow::response & /*res*/, context & /*ctx*/)
    {
        CROW_LOG_DEBUG << " - MESSAGE: " << message;
    }

    void after_handle(crow::request & /*req*/, crow::response & /*res*/, context & /*ctx*/)
    {
        // no-op
    }
};


#endif //CROW_ALL_DOMAIN_H
