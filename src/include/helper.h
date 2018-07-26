//
// Created by Gao,Pengfei(FFTD) on 2018/6/7.
//

#ifndef CROW_ALL_HELPER_H
#define CROW_ALL_HELPER_H

#include <sstream>
#include "crow.h"
#include "crow.h"
#include "crow/json.h"
#include "DomainException.h"
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>
#include <libvirt/libvirt-qemu.h>
#include <functional>


#endif //CROW_ALL_HELPER_H


#define LIBVIRT_SUCCESS 200

#define LIBVIRT_CONNECT_FAILED 100
#define PARAMETER_INVALID 101
#define DOMAIN_DEFINITION_FAILED 102
#define DOMAIN_CANNOT_BOOT 103
#define GET_NODE_INFO_FAILED 104
#define LIST_DOMAIN_FAILED 105
#define GET_DOMAIN_INFO_FAILED 106
#define GET_DOMAIN_PTR_FAILED 107
#define SHUTDOWN_DOMAINFAILD 108
#define NO_DOMAIN 108
#define CANNOT_CHECK_GUEST_STATE 109 //Cannot check guest state
#define CANNOT_SAVE_GUEST 110
#define GET_ABS_PATH_FAIL 111
#define UNABLE_TO_SAVE_DOMAIN 112
#define UNABLE_TO_RESTORE_DOMAIN 113
#define UNABLE_TO_FIND_DOMAIN_THAT_WAS_RESTORED 113



#define DIRCTORY_SAPERATOR std::string("/")
#define GET_MESSAGE(e) *boost::get_error_info<err_str>(e)
#define GET_CODE(e) *boost::get_error_info<err_no>(e)

#define KAEL_TRY try{
#define KAEL_CATCH(x) }catch(x){
#define KAEL_RETHROW throw;
#define KAEL_CATCH_END }
#define KEAL_SHOW_EXCEPTION_JSON(json_object) \
        json_object["code"] = GET_CODE(e); \
        json_object["message"] = GET_MESSAGE(e);


#define KEAL_ADD_MSG_TO_JSON(json_object) \
        json_object["code"] = LIBVIRT_SUCCESS; \
        json_object["message"] = "success";

#define KEAL_EMPTY_CHECK(param) \
        if(!param){ \
                throw DomainException(PARAMETER_INVALID,  "Parameter can`t be empty!!"); \
        }


#define KEAL_THROW_EXP(expr,exception,code,msg) \
        if(expr){ \
                throw exception(code,  msg); \
        }



#define KAEL_ROUTE(app,path,callback,method,...) \
    CROW_ROUTE(app,path) \
    .methods(#method ##_method) \
    ([](const crow::request& req, ##__VA_ARGS__) { \
        return  callback; \
    });

#define KEAL_ROUTE_T(app,path,callback,method,...) \
    KAEL_ROUTE(app,path,callback,method, ##__VA_ARGS__) \
//    KAEL_ROUTE(app,path,callback,OPTIONS, ##__VA_ARGS__)



#define KAEL_ROUTE_ANY(app,path,callback,...) \
    KAEL_ROUTE(app,path,callback,GET, ##__VA_ARGS__) \
    KAEL_ROUTE(app,path,callback,POST, ##__VA_ARGS__) \
    KAEL_ROUTE(app,path,callback,PUT, ##__VA_ARGS__) \
    KAEL_ROUTE(app,path,callback,DELETE, ##__VA_ARGS__) \
    KAEL_ROUTE(app,path,callback,OPTIONS, ##__VA_ARGS__)

typedef boost::error_info<struct tag_err_no, int> err_no;
typedef boost::error_info<struct tag_err_str, std::string> err_str;
