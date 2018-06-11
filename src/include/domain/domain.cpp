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
        list["data"]["host"] = host;
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
        virDomainInfoPtr info;
        virConnectPtr conn = get_virconnetctptr(req.url_params.get("uri"));
        virDomainPtr *domains;
        unsigned int flags = VIR_CONNECT_LIST_DOMAINS_RUNNING |
                             VIR_CONNECT_LIST_DOMAINS_PERSISTENT;
        int ret = virConnectListAllDomains(conn, &domains, flags);
        KEAL_THROW_EXP(ret < 0,DomainException,LIST_DOMAIN_FAILED,"list domain failed!!")
        list["total"] = ret;
        for (size_t i = 0; i < ret; i++) {
            virDomainGetInfo(domains[i],info);
            list["data"][i]["info"]["memory"]       = info->memory;
            list["data"][i]["info"]["nrVirtCpu"]    = info->nrVirtCpu;
            list["data"][i]["info"]["maxMem"]       = info->maxMem;
            list["data"][i]["info"]["cpuTime"]      = info->cpuTime;
            list["data"][i]["info"]["state"]        = info->state;
            list["data"][i]["name"] = virDomainGetHostname(domains[i],0);
            list["data"][i]["id"] = virDomainGetID(domains[i]);
            list["data"][i]["max_memory"] = virDomainGetMaxMemory(domains[i]);
            list["data"][i]["max_vcpus"] = virDomainGetMaxVcpus(domains[i]);
            list["data"][i]["max_memory"] = virDomainGetMaxMemory(domains[i]);
            virDomainFree(domains[i]);
        }
        virConnectClose(conn);
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}

crow::json::wvalue Domain::get_nodeinfo(const crow::request &req) {
    crow::json::wvalue list;
    virNodeInfoPtr info;
    virConnectPtr conn;
    KAEL_TRY
        conn = get_virconnetctptr(req.url_params.get("uri"));
        int ret = virNodeGetInfo(conn,info);
        KEAL_THROW_EXP(ret == -1,DomainException,GET_NODE_INFO_FAILED,"get nodeinfo failed!!")
        list["data"]["memory"]      = info->memory;
        list["data"]["cores"]       = info->cores;
        list["data"]["cpus"]        = info->cpus;
        list["data"]["mhz"]         = info->mhz;
        list["data"]["model"]       = info->model;
        list["data"]["nodes"]       = info->nodes;
        list["data"]["sockets"]     = info->sockets;
        list["data"]["threads"]     = info->threads;
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}


crow::json::wvalue Domain::create_domain(const crow::request &req) {
    crow::json::wvalue list;
    virDomainPtr domain;
    virDomainInfoPtr info;
    KAEL_TRY
        virConnectPtr conn = get_virconnetctptr(req.url_params.get("uri"));
        auto jsonObject = crow::json::load(req.body);
        KEAL_EMPTY_CHECK(jsonObject)
        KEAL_EMPTY_CHECK(jsonObject["xmlconfig"])
        char *xmlconfig = jsonObject["xmlconfig"].s().s_;
        domain = virDomainDefineXML(conn, xmlconfig);
        KEAL_THROW_EXP(!domain,DomainException,DOMAIN_DEFINITION_FAILED,"Domain definition failed")
        KEAL_THROW_EXP(virDomainCreate(domain) < 0,DomainException,DOMAIN_CANNOT_BOOT,"Cannot boot guest")
        list["data"]["domain_id"]   = virDomainGetID(domain);
        virDomainGetInfo(domain,info);
        list["data"]["state"]       = info->state;
        list["data"]["cpuTime"]     = info->cpuTime;
        list["data"]["maxMem"]      = info->maxMem;
        list["data"]["memory"]      = info->memory;
        list["data"]["nrVirtCpu"]   = info->nrVirtCpu;
        virDomainFree(domain);
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
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