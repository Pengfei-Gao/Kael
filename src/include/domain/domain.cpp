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
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}

crow::json::wvalue Domain::get_hostinfo(const crow::request& req) {
    crow::json::wvalue list;
    unsigned long * libVer = NULL;
    KAEL_TRY
        virConnectPtr conn = get_virconnetctptr(req.url_params.get("uri"));
        list["data"]["hostname"] = virConnectGetHostname(conn);
        virConnectGetLibVersion(conn,libVer);
        list["data"]["libvirt_version"] = libVer;
        list["data"]["maxcpus"] = virConnectGetMaxVcpus(conn,"");
        list["data"]["sysinfo"] = virConnectGetSysinfo(conn,0);
        virConnectClose(conn);
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
        virDomainPtr *domains;
        unsigned int flags = VIR_CONNECT_LIST_DOMAINS_RUNNING |
                             VIR_CONNECT_LIST_DOMAINS_PERSISTENT;
        int ret = virConnectListAllDomains(conn, &domains, flags);
        KEAL_THROW_EXP(ret < 0,DomainException,LIST_DOMAIN_FAILED,"list domain failed!!")
        list["total"] = ret;
        for (int i = 0; i < ret; i++) {
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

crow::json::wvalue Domain::get_domain_info(const crow::request &req, int domain_id) {
    crow::json::wvalue list;
    virConnectPtr conn;
    KAEL_TRY
        unsigned char uuid[VIR_UUID_STRING_BUFLEN] = { 0 };
        char uuidstr[VIR_UUID_STRING_BUFLEN] = { 0 };
        conn = get_virconnetctptr(req.url_params.get("uri"));
        virDomainPtr domainPtr = virDomainLookupByID(conn,domain_id);
        int ret = virDomainGetUUID(domainPtr,uuid);
        if(ret == 0){
            list["data"]["uuid"] = uuid;
        }
        ret = virDomainGetUUIDString(domainPtr,uuidstr);
        if(ret == 0){
            list["data"]["uuidstr"] = uuidstr;
        }
        virDomainInfo domainInfo;;
        ret = virDomainGetInfo(domainPtr,&domainInfo);
        if(ret == 0 ){
            list["data"]["state"]       = (long)domainInfo.state;
            list["data"]["cpuUsed"]     = ((double)domainInfo.cpuTime/1000000000.0);
            list["data"]["maxMem"]      = domainInfo.maxMem;
            list["data"]["memory"]      = domainInfo.memory;
            list["data"]["nrVirtCpu"]   = domainInfo.nrVirtCpu;
        }
        list["data"]["domain_id"]                   = domain_id;
        list["data"]["is_active"]                   = virDomainIsActive(domainPtr);
        list["data"]["is_persistent"]               = virDomainIsPersistent(domainPtr);
        list["data"]["is_update"]                   = virDomainIsUpdated(domainPtr);
        list["data"]["name"]                        = virDomainGetName(domainPtr);
        list["data"]["max_memory"]                  = virDomainGetMaxMemory(domainPtr);
        list["data"]["max_vcpus"]                   = virDomainGetMaxVcpus(domainPtr);
        list["data"]["max_memory"]                  = virDomainGetMaxMemory(domainPtr);
        list["data"]["VIR_DOMAIN_XML_SECURE"]       = virDomainGetXMLDesc(domainPtr,1); //dump security sensitive information too
        list["data"]["VIR_DOMAIN_XML_INACTIVE"]     = virDomainGetXMLDesc(domainPtr,2); //dump inactive domain information
        list["data"]["VIR_DOMAIN_XML_UPDATE_CPU"]   = virDomainGetXMLDesc(domainPtr,4); //update guest CPU requirements according to host CPU
        list["data"]["VIR_DOMAIN_XML_MIGRATABLE"]   = virDomainGetXMLDesc(domainPtr,8); //dump XML suitable for migration
        virDomainFree(domainPtr);
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}
crow::json::wvalue Domain::get_nodeinfo(const crow::request &req) {
    crow::json::wvalue list;
    virConnectPtr conn;
    KAEL_TRY
        conn = get_virconnetctptr(req.url_params.get("uri"));
        virNodeInfo info;
        int ret = virNodeGetInfo(conn,&info);
        KEAL_THROW_EXP(ret == -1,DomainException,GET_NODE_INFO_FAILED,"get nodeinfo failed!!")
        list["data"]["memory"]      = info.memory;
        list["data"]["cores"]       = info.cores;
        list["data"]["cpus"]        = info.cpus;
        list["data"]["mhz"]         = info.mhz;
        list["data"]["model"]       = info.model;
        list["data"]["nodes"]       = info.nodes;
        list["data"]["sockets"]     = info.sockets;
        list["data"]["threads"]     = info.threads;
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}

//brctl addbr br0
//virt-install --name="testvm" --ram=2048  --os-type=linux --hvm --cdrom="/opt/Fedora-Server-dvd-x86_64-27-1.6.iso" --file="/opt/linux.img" --file-size=20 --bridge=bridge --vnc --vncport=8080

crow::json::wvalue Domain::create_domain(const crow::request &req) {
    crow::json::wvalue list;
    virDomainPtr domain;
    virDomainInfo info;
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
        virDomainGetInfo(domain,&info);
        list["data"]["state"]       = info.state;
        list["data"]["cpuTime"]     = info.cpuTime;
        list["data"]["maxMem"]      = info.maxMem;
        list["data"]["memory"]      = info.memory;
        list["data"]["nrVirtCpu"]   = info.nrVirtCpu;
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