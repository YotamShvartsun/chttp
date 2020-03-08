#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <typeinfo>

#include <chttp/data/HttpRequest.h>
#include <chttp/data/HttpResponse.h>
#include <chttp/util/Url.h>

using RequestType_t = HttpRequest::REQUEST_TYPES;
typedef std::function<void(std::shared_ptr<HttpRequest>,
                           std::shared_ptr<HttpResponse>)>
    RequestHandlerFunction;
class RequestHandler {
private:
  Url urlMatch;
  RequestHandlerFunction handler;
  RequestType_t requestType = RequestType_t::GET;

public:
  RequestHandler(RequestType_t type, RequestHandlerFunction handler,
                 const Url &urlMatch);

  bool IsMatching(const std::shared_ptr<HttpRequest> &) const;

  void operator()(const std::shared_ptr<HttpRequest> &,
                  std::shared_ptr<HttpResponse>);

  static std::unordered_map<int, std::string> CreateParamMap(std::string);
};