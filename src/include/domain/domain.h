//
// Created by Gao,Pengfei(FFTD) on 2018/6/6.
//

#ifndef CROW_ALL_DOMAIN_H
#define CROW_ALL_DOMAIN_H


#include "helper.h"

class Domain {
public:
    static crow::json::wvalue getHostname(const crow::request& );
    static crow::json::wvalue getHostInfo(const crow::request& );
    static crow::json::wvalue getNodeInfo(const crow::request& );
    static crow::json::wvalue getDomainInfoById(const crow::request& ,int );
    static crow::json::wvalue getDomainInfoByUuidOrName(const crow::request& ,std::string );
    static crow::json::wvalue getDomainInfoByDomainPtr(virDomainPtr,bool isNeedXML);

    static crow::json::wvalue listDomain(const crow::request& );
    static crow::json::wvalue listAllDomain(const crow::request& );
    static crow::json::wvalue createDomain(const crow::request& );
    static crow::json::wvalue shutdownByUuidOrname(const crow::request& ,std::string);
    static crow::json::wvalue destroyByUuidOrname(const crow::request& ,std::string);
    static crow::json::wvalue shutdownById(const crow::request& ,int );
    static crow::json::wvalue destroyById(const crow::request& ,int );
    static crow::json::wvalue startById(const crow::request& ,int );
    static crow::json::wvalue startByUuidOrname(const crow::request& ,std::string );


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




#endif //CROW_ALL_DOMAIN_H
