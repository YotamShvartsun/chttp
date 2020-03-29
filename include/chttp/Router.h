/**
 * @file
 * @author Yotam Shvartsun <yotam.shvartsun@gmail.com>
 * @version 1.0
 * @section DESCRIPTION
 * This file contains the definitions for the Router class
 */
#pragma once

#include <chttp/RequestHandler.h>
#include <chttp/data/HttpRequest.h>
#include <chttp/data/HttpResponse.h>
#include <iostream>
#include <vector>

/**
 * This class
 */
class Router {
private:
  /**
   * A list of all routes in the server
   */
  std::vector<RequestHandler> routes;
  /**
   * This is a singleton, so the instance must be stored in a private static
   * field
   */
  static Router *instance;
  /**
   * The 404 page handler
   */
  RequestHandlerFunction notFoundHandler;
  /**
   * The error page handler
   */
  std::function<void(std::shared_ptr<HttpRequest>,
                     std::shared_ptr<HttpResponse>, std::string)>
      errorHandler;
  /**
   * Create a new router
   * @param routes A list of all routes
   * @param notFoundHandler A custom 404-error handler
   * @param errorHandler A custom general-error handler
   */
  Router(std::vector<RequestHandler> routes,
         RequestHandlerFunction notFoundHandler,
         std::function<void(std::shared_ptr<HttpRequest>,
                            std::shared_ptr<HttpResponse>, std::string)>
             errorHandler

  );
  /**
   * Create a new router, with default error pages
   * @param routes A list of routes
   */
  explicit Router(std::vector<RequestHandler> routes);
  /**
   * The default 404-error handler
   */
  static void DefaultNotFound(const std::shared_ptr<HttpRequest> &,
                              const std::shared_ptr<HttpResponse> &);
  /**
   * The default general-error page
   */
  static void DefaultError(const std::shared_ptr<HttpRequest> &,
                           const std::shared_ptr<HttpResponse> &,
                           const std::string &);

public:
  /**
   * Get an instance of the router class, with default error pages
   * @param routes The route list
   * @return An instance of the router class
   */
  static Router *GetInstance(std::vector<RequestHandler> routes);
  /**
   * Get the existing instance of the router class
   * @return An instance of the router class
   */
  static Router *GetInstance();
  /**
   * Create an instance of the router class, with no default values
   * @param routes The route list
   * @param notFoundHandler The 404-error handler
   * @param errorHandler The general-error handler
   * @return A new instance of the router class
   */
  static Router *GetInstance(
      const std::vector<RequestHandler> &routes,
      const RequestHandlerFunction &notFoundHandler,
      const std::function<void(std::shared_ptr<HttpRequest>,
                               std::shared_ptr<HttpResponse>, std::string)>
          &errorHandler

  );
  /**
   * Add a new handler to the router class
   * @param route The handler to add
   */
  void AddHandler(const RequestHandler &route);
  /**
   * Match a request to the handler or to the error pages
   */
  void Route(const std::shared_ptr<HttpRequest> &,
             const std::shared_ptr<HttpResponse> &);
  /**
   * Set the instance to nullptr
   */
  static void Reset();
};