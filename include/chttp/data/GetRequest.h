#pragma once

#include "chttp/data/HttpRequest.h"

class GetRequest :  HttpRequest {
public:
    GetRequest(std::vector<char> data);
    virtual void FromString(std::vector<char> data);
};