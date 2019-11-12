#pragma once

#include "chttp/data/HttpRequest.h"

class GetRequest : public HttpRequest {
public:
    GetRequest(std::vector<char> data);
    GetRequest & operator=(const GetRequest &);
    virtual void FromString(std::vector<char> data);
};