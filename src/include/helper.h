//
// Created by Gao,Pengfei(FFTD) on 2018/6/7.
//

#ifndef CROW_ALL_HELPER_H
#define CROW_ALL_HELPER_H

#endif //CROW_ALL_HELPER_H


#define LIBVIRT_SUCCESS 0

#define LIBVIRT_CONNECT_FAILED 100
#define PARAMETER_INVALID 101
#define DOMAIN_DEFINITION_FAILED 102
#define DOMAIN_CANNOT_BOOT 103




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
                throw DomainException(PARAMETER_INVALID,  "Parameter param can`t be empty!!"); \
        }


#define KEAL_THROW_EXP(expr,exception,code,msg) \
        if(expr){ \
                throw exception(code,  msg); \
        }


//__VA_ARGS__
#define KAEL_ROUTE(app,path,callback,...) \
    CROW_ROUTE(app,path) \
    ([](const crow::request& req,##__VA_ARGS__) { \
        return  callback; \
    });

typedef boost::error_info<struct tag_err_no, int> err_no;
typedef boost::error_info<struct tag_err_str, std::string> err_str;
