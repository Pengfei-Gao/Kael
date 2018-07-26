//
// Created by Gao,Pengfei(FFTD) on 2018/6/6.
//

#include "domain/domain.h"


crow::json::wvalue Domain::getHostname(const crow::request& req) {
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

crow::json::wvalue Domain::getHostInfo(const crow::request& req) {

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

crow::json::wvalue Domain::listAllDomain(const crow::request &req) {
    crow::json::wvalue list;
    int count = -1;
    int expectedcount = -1;
    int *ids;
    virDomainPtr domain = NULL;
    KAEL_TRY
        virConnectPtr conn = get_virconnetctptr(req.url_params.get("uri"));
        KEAL_THROW_EXP((expectedcount = virConnectNumOfDomains(conn)) < 0,DomainException,NO_DOMAIN,"No domain!!")
        ids = (int *)malloc(sizeof(int)*expectedcount);
        count = virConnectListDomains(conn, ids, expectedcount);
        for(int i = 0 ; i < count; i ++ ){
            domain = virDomainLookupByID(conn, ids[i]);
            if(domain != NULL){
                list["data"][i] = getDomainInfoByDomainPtr(domain,false);
            }
        }
        virConnectClose(conn);
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}

crow::json::wvalue Domain::listDomain(const crow::request& req) {
    crow::json::wvalue list;
    KAEL_TRY
        virConnectPtr conn = get_virconnetctptr(req.url_params.get("uri"));
        virDomainPtr *domains;
        unsigned int flags = VIR_CONNECT_LIST_DOMAINS_PERSISTENT;
        int ret = virConnectListAllDomains(conn, &domains, flags);
        KEAL_THROW_EXP(ret < 0,DomainException,LIST_DOMAIN_FAILED,"list domain failed!!")
        list["total"] = ret;
        for (int i = 0; i < ret; i++) {
            list["data"][i] = getDomainInfoByDomainPtr(domains[i],false);
            virDomainFree(domains[i]);
        }
        virConnectClose(conn);
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}

crow::json::wvalue Domain::getDomainInfoById(const crow::request &req, int domain_id) {
    crow::json::wvalue list;
    virConnectPtr conn;
    KAEL_TRY
        conn = get_virconnetctptr(req.url_params.get("uri"));
        virDomainPtr domainPtr = virDomainLookupByID(conn,domain_id);
        KEAL_THROW_EXP(domainPtr == NULL,DomainException,GET_DOMAIN_PTR_FAILED,"GET domain failed!!")
        list["data"] = getDomainInfoByDomainPtr(domainPtr,true);
        virDomainFree(domainPtr);
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}

crow::json::wvalue Domain::getDomainInfoByUuidOrName(const crow::request &req, std::string uuidOrname) {
    crow::json::wvalue list;
    virConnectPtr conn;
    KAEL_TRY
        conn = get_virconnetctptr(req.url_params.get("uri"));
        virDomainPtr domain = virDomainLookupByName(conn, uuidOrname.c_str());
        if(domain == NULL){
            domain = virDomainLookupByUUID(conn,(const unsigned char *)uuidOrname.c_str());
        }
        KEAL_THROW_EXP(domain == NULL,DomainException,GET_DOMAIN_PTR_FAILED,"GET domain failed!!")
        list["data"] = getDomainInfoByDomainPtr(domain,true);
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}

crow::json::wvalue Domain::getNodeInfo(const crow::request &req) {
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

crow::json::wvalue Domain::createDomain(const crow::request &req) {
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
        list["data"]   = getDomainInfoByDomainPtr(domain,true);
        virDomainGetInfo(domain,&info);
        virDomainFree(domain);
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}

crow::json::wvalue Domain::shutdownByUuidOrname(const crow::request &req,std::string uuidOrname) {
    crow::json::wvalue list;
    KAEL_TRY
        virConnectPtr conn = get_virconnetctptr(req.url_params.get("uri"));
        virDomainPtr domainPtr = virDomainLookupByName(conn, uuidOrname.c_str());
        if(domainPtr == NULL){
            domainPtr = virDomainLookupByUUID(conn,(const unsigned char *)uuidOrname.c_str());
        }
        KEAL_THROW_EXP(domainPtr == NULL,DomainException,GET_DOMAIN_PTR_FAILED,"GET domain failed!!")
        int ret = virDomainShutdown(domainPtr);
        KEAL_THROW_EXP(ret == -1,DomainException,SHUTDOWN_DOMAINFAILD,"shutdown domain failed!!")
        list = getDomainInfoByDomainPtr(domainPtr,false);
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}

crow::json::wvalue Domain::destroyByUuidOrname(const crow::request &req,std::string uuidOrname) {
    crow::json::wvalue list;
    KAEL_TRY
        virConnectPtr conn = get_virconnetctptr(req.url_params.get("uri"));
        virDomainPtr domainPtr = virDomainLookupByName(conn, uuidOrname.c_str());
        if(domainPtr == NULL){
            domainPtr = virDomainLookupByUUID(conn,(const unsigned char *)uuidOrname.c_str());
        }
        KEAL_THROW_EXP(domainPtr == NULL,DomainException,GET_DOMAIN_PTR_FAILED,"GET domain failed!!")
        int ret = virDomainDestroy(domainPtr);
        KEAL_THROW_EXP(ret == -1,DomainException,SHUTDOWN_DOMAINFAILD,"shutdown domain failed!!")
        list = getDomainInfoByDomainPtr(domainPtr,false);
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}


crow::json::wvalue Domain::startById(const crow::request &req, int id) {
    crow::json::wvalue list;
    KAEL_TRY
        virConnectPtr conn = get_virconnetctptr(req.url_params.get("uri"));
        virDomainPtr domainPtr = virDomainLookupByID(conn,id);
        KEAL_THROW_EXP(domainPtr == NULL,DomainException,GET_DOMAIN_PTR_FAILED,"GET domain failed!!")
        KEAL_THROW_EXP(virDomainCreate(domainPtr) < 0,DomainException,DOMAIN_CANNOT_BOOT,"Cannot boot guest")
        list = getDomainInfoByDomainPtr(domainPtr,false);
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}

crow::json::wvalue Domain::startByUuidOrname(const crow::request &req, std::string uuidOrname) {
    crow::json::wvalue list;
    virConnectPtr conn;
    KAEL_TRY
        conn = get_virconnetctptr(req.url_params.get("uri"));
        virDomainPtr domainPtr = virDomainLookupByName(conn, uuidOrname.c_str());
        if(domainPtr == NULL){
            domainPtr = virDomainLookupByUUID(conn,(const unsigned char *)uuidOrname.c_str());
        }
        KEAL_THROW_EXP(domainPtr == NULL,DomainException,GET_DOMAIN_PTR_FAILED,"GET domain failed!!")
        KEAL_THROW_EXP(virDomainCreate(domainPtr) < 0,DomainException,DOMAIN_CANNOT_BOOT,"Cannot boot guest")
        list["data"] = getDomainInfoByDomainPtr(domainPtr,false);
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}

crow::json::wvalue Domain::shutdownById(const crow::request &req,int id) {
    crow::json::wvalue list;
    KAEL_TRY
        virConnectPtr conn = get_virconnetctptr(req.url_params.get("uri"));
        virDomainPtr domainPtr = virDomainLookupByID(conn,id);
        KEAL_THROW_EXP(domainPtr == NULL,DomainException,GET_DOMAIN_PTR_FAILED,"GET domain failed!!")
        int ret = virDomainDestroy(domainPtr);
        KEAL_THROW_EXP(ret == -1,DomainException,SHUTDOWN_DOMAINFAILD,"shutdown domain failed!!")
        list = getDomainInfoByDomainPtr(domainPtr,false);
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}

crow::json::wvalue Domain::destroyById(const crow::request &req,int id) {
    crow::json::wvalue list;
    KAEL_TRY
        virConnectPtr conn = get_virconnetctptr(req.url_params.get("uri"));
        virDomainPtr domainPtr = virDomainLookupByID(conn,id);
        KEAL_THROW_EXP(domainPtr == NULL,DomainException,GET_DOMAIN_PTR_FAILED,"GET domain failed!!")
        int ret = virDomainDestroy(domainPtr);
        KEAL_THROW_EXP(ret == -1,DomainException,SHUTDOWN_DOMAINFAILD,"shutdown domain failed!!")
        list = getDomainInfoByDomainPtr(domainPtr,false);
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

crow::json::wvalue Domain::getDomainInfoByDomainPtr(virDomainPtr domainPtr,bool isNeedXML) {
    crow::json::wvalue list;
    unsigned char uuid[VIR_UUID_STRING_BUFLEN] = { 0 };
    char uuidstr[VIR_UUID_STRING_BUFLEN] = { 0 };
    int ret = virDomainGetUUID(domainPtr,uuid);
    if(ret == 0){
        list["uuid"] = uuid;
    }
    ret = virDomainGetUUIDString(domainPtr,uuidstr);
    if(ret == 0){
        list["uuidStr"] = uuidstr;
    }
    virDomainInfo domainInfo;;
    ret = virDomainGetInfo(domainPtr,&domainInfo);
    if(ret == 0 ){
        list["state"]       = (long)domainInfo.state;
        list["cpuUsed"]     = ((double)domainInfo.cpuTime/1000000000.0);
        list["maxMem"]      = domainInfo.maxMem;
        list["memory"]      = domainInfo.memory;
        list["nrVirtCpu"]   = domainInfo.nrVirtCpu;
    }
    list["id"]                   = virDomainGetID(domainPtr);
    list["isActive"]                   = virDomainIsActive(domainPtr);
    list["isPersistent"]               = virDomainIsPersistent(domainPtr);
    list["isUpdate"]                   = virDomainIsUpdated(domainPtr);
    list["name"]                        = virDomainGetName(domainPtr);
    list["maxMemory"]                  = virDomainGetMaxMemory(domainPtr);
    list["maxVcpus"]                   = virDomainGetMaxVcpus(domainPtr);
    list["maxMemory"]                  = virDomainGetMaxMemory(domainPtr);
    list["osType"]                     = virDomainGetOSType(domainPtr);
    if(isNeedXML){
        list["VIR_DOMAIN_XML_SECURE"]       = virDomainGetXMLDesc(domainPtr,1); //dump security sensitive information too
        list["VIR_DOMAIN_XML_INACTIVE"]     = virDomainGetXMLDesc(domainPtr,2); //dump inactive domain information
        list["VIR_DOMAIN_XML_UPDATE_CPU"]   = virDomainGetXMLDesc(domainPtr,4); //update guest CPU requirements according to host CPU
        list["VIR_DOMAIN_XML_MIGRATABLE"]   = virDomainGetXMLDesc(domainPtr,8); //dump XML suitable for migration
    }
    return list;
}

crow::json::wvalue Domain::saveById(const crow::request &req, int id) {
    crow::json::wvalue list;
    KAEL_TRY
        virDomainInfoPtr info = NULL;
        boost::filesystem::path current_path = boost::filesystem::current_path();
        virConnectPtr conn = get_virconnetctptr(req.url_params.get("uri"));
        virDomainPtr domainPtr = virDomainLookupByID(conn,id);
        KEAL_THROW_EXP(domainPtr == NULL,DomainException,GET_DOMAIN_PTR_FAILED,"GET domain failed!!")
        KEAL_THROW_EXP(virDomainGetInfo(domainPtr, info) < 0,DomainException,CANNOT_CHECK_GUEST_STATE,"Cannot check guest state")
        KEAL_THROW_EXP(info->state == VIR_DOMAIN_SHUTOFF,DomainException,CANNOT_SAVE_GUEST,"Not saving guest that isn't running")
        KEAL_THROW_EXP(current_path.empty() ,DomainException,GET_ABS_PATH_FAIL,"get path failed!!")
        const char* domainName = virDomainGetName(domainPtr);
        std::string file_path = current_path.string()  + DIRCTORY_SAPERATOR + std::string("domains") + DIRCTORY_SAPERATOR + std::string(domainName);
        KEAL_THROW_EXP(virDomainSave(domainPtr, file_path.c_str()) < 0,DomainException,UNABLE_TO_SAVE_DOMAIN,"Unable to save guest")
        list["file_path"] = file_path;
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}

crow::json::wvalue Domain::saveByUuidOrname(const crow::request &req, std::string uuidOrname) {
    crow::json::wvalue list;
    KAEL_TRY
        virDomainInfoPtr info = NULL;
        boost::filesystem::path current_path = boost::filesystem::current_path();
        virConnectPtr conn = get_virconnetctptr(req.url_params.get("uri"));
        virDomainPtr domainPtr = virDomainLookupByName(conn, uuidOrname.c_str());
        if(domainPtr == NULL){
            domainPtr = virDomainLookupByUUID(conn,(const unsigned char *)uuidOrname.c_str());
        }
        KEAL_THROW_EXP(domainPtr == NULL,DomainException,GET_DOMAIN_PTR_FAILED,"GET domain failed!!")
        KEAL_THROW_EXP(virDomainGetInfo(domainPtr, info) < 0,DomainException,CANNOT_CHECK_GUEST_STATE,"Cannot check guest state")
        KEAL_THROW_EXP(info->state == VIR_DOMAIN_SHUTOFF,DomainException,CANNOT_SAVE_GUEST,"Not saving guest that isn't running")
        KEAL_THROW_EXP(current_path.empty() ,DomainException,GET_ABS_PATH_FAIL,"get path failed!!")
        const char* domainName = virDomainGetName(domainPtr);
        std::string file_path = current_path.string()  + DIRCTORY_SAPERATOR + std::string("domains") + DIRCTORY_SAPERATOR + std::string(domainName);
        KEAL_THROW_EXP(virDomainSave(domainPtr, file_path.c_str()) < 0,DomainException,UNABLE_TO_SAVE_DOMAIN,"Unable to save guest")
        list["file_path"] = file_path;
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}


crow::json::wvalue Domain::restore(const crow::request &req) {
    crow::json::wvalue list;
    KAEL_TRY
        auto Params =  crow::json::load(req.body);
        virConnectPtr conn = get_virconnetctptr(req.url_params.get("uri"));
        int id = virDomainRestore(conn, Params["path"].s().s_);
        KEAL_THROW_EXP(id < 0,DomainException,UNABLE_TO_RESTORE_DOMAIN,"Unable to restore guest")
        virDomainPtr domainPtr = virDomainLookupByID(conn, id);
        KEAL_THROW_EXP(domainPtr == NULL,DomainException,UNABLE_TO_FIND_DOMAIN_THAT_WAS_RESTORED,"Cannot find guest that was restored")
        list = getDomainInfoByDomainPtr(domainPtr,true);
        KEAL_ADD_MSG_TO_JSON(list);
    KAEL_CATCH(DomainException e)
        KEAL_SHOW_EXCEPTION_JSON(list);
    KAEL_CATCH_END
    return list;
}
