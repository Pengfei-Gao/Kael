//
// Created by Gao,Pengfei(FFTD) on 2018/6/7.
//

#ifndef CROW_ALL_DOMAINEXCEPTION_H
#define CROW_ALL_DOMAINEXCEPTION_H
#include <boost/exception/all.hpp>
#include "helper.h"
#include "crow.h"

class DomainException: virtual public std::exception,virtual public boost::exception{

public:
    DomainException(int ,const char*);
    ~DomainException();
};


#endif //CROW_ALL_DOMAINEXCEPTION_H
