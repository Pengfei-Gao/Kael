//
// Created by Gao,Pengfei(FFTD) on 2018/6/7.
//

#include "DomainException.h"


DomainException::DomainException(int code, const char *msg) {
    (*this) << err_no(code) << err_str(msg);
}

DomainException::~DomainException() {

}