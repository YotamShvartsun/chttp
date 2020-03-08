#pragma once

#include <chttp/Handler.h>
#include <chttp/data/HttpRequest.h>
#include <chttp/data/HttpResponse.h>
#include <iostream>
#include <vector>

class Router {
private:
  std::vector<RequestHandler> routes;
  static Router *instance;
  RequestHandlerFunction notFoundHandler;
  std::function<void(std::shared_ptr<HttpRequest>,
                     std::shared_ptr<HttpResponse>, std::string)>
      errorHandler;

  Router(std::vector<RequestHandler> routes,
         RequestHandlerFunction notFoundHandler,
         std::function<void(std::shared_ptr<HttpRequest>,
                            std::shared_ptr<HttpResponse>, std::string)>
             errorHandler

  );

  explicit Router(std::vector<RequestHandler> routes);

  static void DefaultNotFound(const std::shared_ptr<HttpRequest> &,
                              const std::shared_ptr<HttpResponse> &);

  static void DefaultError(const std::shared_ptr<HttpRequest> &,
                           const std::shared_ptr<HttpResponse> &,
                           const std::string &);

public:
  static Router *GetInstance(std::vector<RequestHandler> routes);

  static Router *GetInstance();

  static Router *GetInstance(
      const std::vector<RequestHandler> &routes,
      const RequestHandlerFunction &notFoundHandler,
      const std::function<void(std::shared_ptr<HttpRequest>,
                               std::shared_ptr<HttpResponse>, std::string)>
          &errorHandler

  );
  void AddHandler(const RequestHandler &route);
  void Route(const std::shared_ptr<HttpRequest> &,
             const std::shared_ptr<HttpResponse> &);
  static void Reset();
};