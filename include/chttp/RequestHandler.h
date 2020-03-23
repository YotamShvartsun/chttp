/**
 * @file
 * @author Yotam Shvartsun <yotam.shvartsun@gmail.com>
 * @version 1.0
 * @section DESCRIPTION
 * This file contains the definitions for the RequestHandler class, which
 * represent a route and its user-defined handler
 */
#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <typeinfo>

#include <chttp/data/HttpRequest.h>
#include <chttp/data/HttpResponse.h>
#include <chttp/util/Url.h>

// Defining types instead of writing long typenames/templates
using RequestType_t = HttpRequest::REQUEST_TYPES;
typedef std::function<void(std::shared_ptr<HttpRequest>,
                           std::shared_ptr<HttpResponse>)>
    RequestHandlerFunction;

/**
 * This class represents a route, with a URL-template and a handler
 */
class RequestHandler {
private:
  /**
   * The url assigned to this handler
   */
  Url urlMatch;
  /**
   * The handler function
   */
  RequestHandlerFunction handler;
  /**
   * Request type expected
   */
  RequestType_t requestType = RequestType_t::GET;

public:
  /**
   * Create a new instance of the class
   * @param type Type of the request
   * @param handler A function responsible for client-handling
   * @param urlMatch The URL-template
   */
  RequestHandler(RequestType_t type, RequestHandlerFunction handler,
                 const Url &urlMatch);
  /**
   * This function checks if this handler matches the request
   * @return If the request matches, true. Else, false
   */
  bool IsMatching(const std::shared_ptr<HttpRequest> &) const;
  /**
   * Call the handler with the request and the response objects
   * @note The handler won't be called if the request dose not match
   */
  void operator()(const std::shared_ptr<HttpRequest> &,
                  std::shared_ptr<HttpResponse>);
  /**
   * Create parameter map (for a Url object) based on a string template
   * @example: the url '/:name/admin' will return [{0:"name"}, {1: "admin"}]
   * @return the Url-template
   */
  static std::unordered_map<int, std::string> CreateParamMap(std::string);
};