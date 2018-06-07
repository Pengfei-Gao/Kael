//
// Created by Gao,Pengfei(FFTD) on 2018/6/6.
//

#include "domain/domain.h"

crow::json::wvalue Domain::get_hostname(const crow::request& req) {
    crow::json::wvalue list;
    char *host;
    KAEL_TRY
        virConnectPtr conn = get_virconnetctptr(req.url_params.get("uri"));
        host = virConnectGetHostname(conn);
        virConnectClose(conn);
        list["host"] = host;
        free(host);
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}

crow::json::wvalue Domain::list_domain(const crow::request& req) {
    crow::json::wvalue list;
    KAEL_TRY
        virConnectPtr conn = get_virconnetctptr(req.url_params.get("uri"));
        int numDomains = (int)virConnectNumOfDomains(conn);

        int *activeDomains = new int[numDomains];

        numDomains = virConnectListDomains(conn, activeDomains, numDomains);

        for (int i = 0; i < numDomains; i++)
        {
            list["domains"][i] = activeDomains[i];
        }
        list["total"] = numDomains;
        virConnectClose(conn);
        free(activeDomains);
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}

crow::json::wvalue Domain::create_domain(const crow::request &req) {
    crow::json::wvalue list;
//    virDomainPtr domain;
//    KAEL_TRY
//        virConnectPtr conn = get_virconnetctptr(req.url_params.get("uri"));
//        auto jsonObject = crow::json::load(req.body);
//        KEAL_EMPTY_CHECK(jsonObject)
//        KEAL_EMPTY_CHECK(jsonObject['xmlconfig'])
//        char *xmlconfig = jsonObject['xmlconfig'].s().s_;
//        domain = virDomainDefineXML(conn, xmlconfig);
//        KEAL_THROW_EXP(!domain,DomainException,DOMAIN_DEFINITION_FAILED,"Domain definition failed")
//        KEAL_THROW_EXP(virDomainCreate(domain) < 0,DomainException,DOMAIN_CANNOT_BOOT,"Cannot boot guest")
//        list['domainname'] = virDomainName(domain);
//        KEAL_ADD_MSG_TO_JSON(list);
//    KAEL_CATCH(DomainException e)
//        KEAL_SHOW_EXCEPTION_JSON(list);
//    KAEL_CATCH_END
//    virDomainFree(domain);
    return list;
}

virConnectPtr Domain::get_virconnetctptr(const char *uri) {
    virConnectPtr conn;
    if(uri != NULL && uri[0]!='\0'){
        conn = virConnectOpen(uri);
    }else{
        uri = "qemu:///system";
        conn = virConnectOpen(uri);
    }
    KEAL_THROW_EXP(conn == NULL,DomainException,LIBVIRT_CONNECT_FAILED,"libvirt connect open error")
    return conn;
}