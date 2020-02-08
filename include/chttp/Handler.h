#pragma once


#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <typeinfo>

#include <chttp/util/Url.h>
#include <chttp/data/HttpRequest.h>
#include <chttp/data/HttpResponse.h>

using RequestType_t = HttpRequest::REQUEST_TYPES;

class RequestHandler {
private:
    Url urlMatch;
    std::function<void(std::shared_ptr<HttpRequest>, std::shared_ptr<HttpResponse>)> handler;
    RequestType_t requestType = RequestType_t::GET;
public:
    RequestHandler(RequestType_t type,
                   std::function<void(std::shared_ptr<HttpRequest>,
                                      std::shared_ptr<
                                              HttpResponse>)> handler, const Url &urlMatch);

    bool IsMatching(const std::shared_ptr<HttpRequest> &) const;

    void operator()(const std::shared_ptr<HttpRequest> &, std::shared_ptr<HttpResponse>);
};