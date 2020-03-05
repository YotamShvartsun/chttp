#include <chttp/Router.h>

#include <utility>

Router *Router::instance = nullptr;

Router *Router::GetInstance(std::vector<RequestHandler> handlers) {
    if (Router::instance != nullptr)
        return Router::instance;
    Router::instance = new Router(std::move(handlers));
    return Router::instance;
}

Router *Router::GetInstance() {
    if (Router::instance != nullptr)
        return Router::instance;
    throw std::runtime_error("Object not initialized");
}

Router *
Router::GetInstance(const std::vector<RequestHandler> &routes, const std::function<void(std::shared_ptr<HttpRequest>,
                                                                                        std::shared_ptr<HttpResponse>)> &notFoundHandler,
                    const std::function<void(std::shared_ptr<HttpRequest>, std::shared_ptr<HttpResponse>,
                                             std::string)> &errorHandler) {
    if (Router::instance != nullptr)
        return Router::instance;
    Router::instance = new Router(routes, notFoundHandler, errorHandler);
    return Router::instance;
}

void Router::Route(const std::shared_ptr<HttpRequest> &req, const std::shared_ptr<HttpResponse> &res) {
    for (auto handler : this->routes) {
        if (handler.IsMatching(req)) {
            try {
                handler(req, res);
            } catch (std::exception &e) {
                this->errorHandler(req, res, e.what());
            }
            return;
        }
    }
    this->notFoundHandler(req, res);
}

Router::Router(std::vector<RequestHandler> routes,
               std::function<void(std::shared_ptr<HttpRequest>, std::shared_ptr<HttpResponse>)> notFoundHandler,
               std::function<void(std::shared_ptr<HttpRequest>, std::shared_ptr<HttpResponse>,
                                  std::string)> errorHandler) {
    this->routes = std::move(routes);
    this->errorHandler = std::move(errorHandler);
    this->notFoundHandler = std::move(notFoundHandler);
}

Router::Router(std::vector<RequestHandler> routes) {
    this->routes = std::move(routes);
    this->notFoundHandler = Router::DefaultNotFound;
    this->errorHandler = Router::DefaultError;
}

void Router::DefaultNotFound(const std::shared_ptr<HttpRequest> &req, const std::shared_ptr<HttpResponse> &res) {
    std::string response = "Url '" + req->GetUrl() + "' was not found";
    std::vector<char> responseBytes(response.begin(), response.end());
    res->SetStatus(HTTP_STATUS::Not_Found);
    res->Raw(responseBytes);
}

void Router::DefaultError(const std::shared_ptr<HttpRequest> &req, const std::shared_ptr<HttpResponse> &res,
                          const std::string &err) {
    std::string responseStr = "Error while serving " + req->GetUrl() + "\n" + err;
    std::vector<char> responseBytes(responseStr.begin(), responseStr.end());
    res->SetStatus(HTTP_STATUS::Internal_Server_Error);
    res->Raw(responseBytes);
}
