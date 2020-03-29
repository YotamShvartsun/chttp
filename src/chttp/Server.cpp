/**
 * @file
 * @author Yotam Shvartsun <yotam.shvartsun@gmail.com>
 * @version 1.0
 * @section DESCRIPTION
 * This file contains the implementation for the Server class, which is the main
 * class of CHttp
 */
#include <chttp/Server.h>

#ifdef __linux__
#include <climits>
#include <cstdlib>
#include <utility>
#endif

void Server::OnClient(Router *router,
                      const std::shared_ptr<Socket> &clientSocket) {
  std::vector<char> data;
  char thisChar = '\0', lastChar;
  size_t numberOfNewlines = 0;
  bool firstLine = true;
  std::shared_ptr<HttpRequest> req;
  RequestType_t type = RequestType_t::GET;
  // Read the request char by char, until there is a \r\n\r\n sequence
  while (numberOfNewlines != 2) {
    lastChar = thisChar;
    try {
      thisChar = clientSocket->GetData(1)[0];
    } catch (
        std::runtime_error &) { // If the socket was closed by the client, stop
                                // reading from it and close the connection
      return;
    }
    data.push_back(thisChar);
    // a new line
    if (lastChar == '\r' && thisChar == '\n') {
      numberOfNewlines++;
      // Find out what is the request type
      if (firstLine) {
        firstLine = false;
        std::string asString(data.begin(), data.end());
        if (asString.find("GET") != std::string::npos) {
          type = RequestType_t::GET;
        } else {
          type = RequestType_t::POST;
        }
      }
    } else {
      if (lastChar != '\n') {
        numberOfNewlines = 0;
      }
    }
  }
  // If it's a post request, read the body as well, and create an instance
  if (RequestType_t::POST == type) {
    std::size_t contentSize =
        std::stoll(HttpRequest::ParseHTTPHeaders(data)["Content-Length"]);
    std::vector<char> body = clientSocket->GetData(contentSize);
    if (contentSize != body.size())
      throw std::runtime_error("Invalid post");
    data.insert(data.end(), body.begin(), body.end());
    req.reset(new PostRequest(data));
  } else
    req.reset(new GetRequest(data));

  std::shared_ptr<HttpResponse> res;
  res.reset(new HttpResponse());
  // Serve the client
  router->Route(req, res);
  // Send the result
  clientSocket->SendData(res->Format());
}
std::function<void(std::shared_ptr<HttpRequest>, std::shared_ptr<HttpResponse>)>
Server::StaticFileHandler() {
  return [this](const std::shared_ptr<HttpRequest> &request,
                const std::shared_ptr<HttpResponse> &response) {
    std::string requestUrl = request->GetUrl();
    std::string filePath =
        this->staticFolderPath + '/' +
        requestUrl.substr(requestUrl.find(this->staticFolderUrl) +
                          this->staticFolderUrl.size());
    response->SendFile(filePath);
  };
}
Server::Server() {
  std::vector<RequestHandler> defaultHandlers;
  this->router = Router::GetInstance(defaultHandlers);
  this->pool = Threadpool::GetInstance(&this->ClientHandler, this->router);
}

void Server::Run(int port, const std::function<void()> OnStart) {
  std::shared_ptr<Socket> client;
  this->port = port;
  this->server.Bind(this->port);
  this->server.Listen();
  OnStart();
  while (true) {
    client = std::make_shared<Socket>(this->server.Accept());
    this->pool->AddWork(client);
  }
}
void Server::ServeStaticFolder(std::string url, std::string folderPath) {
  this->staticFolderUrl = std::move(url);
  // Get absolute path based on the relative path supplied
#ifdef __linux__
  char *pathBuffer = const_cast<char *>(folderPath.c_str());
  char *absolutePath[PATH_MAX + 1] = {0};
  char *ptr = realpath(pathBuffer, reinterpret_cast<char *>(absolutePath));
  // realpath will return NULL for non-successful operation
  if (NULL == ptr) {
    std::cerr << "Directory " << folderPath << " not found!" << std::endl;
    exit(1);
  }
  folderPath = std::string(ptr);
#endif
  this->staticFolderPath = folderPath;
  Url urlMatch(this->staticFolderUrl + "/:*");
  RequestHandler handler(RequestType_t::GET, this->StaticFileHandler(),
                         urlMatch);
  this->router->AddHandler(handler);
}

Server::~Server() {
  this->server.Close();
  delete this->pool;
  delete this->router;
  Threadpool::Reset();
  Router::Reset();
}
void Server::Get(std::string urlTemplate,
                 const RequestHandlerFunction &function,
                 const std::vector<RequestHandlerFunction> &middlewares) {
  Url urlMatch(std::move(urlTemplate));
  RequestHandlerFunction actualFunction =
      [middlewares, function](const std::shared_ptr<HttpRequest> &req,
                              const std::shared_ptr<HttpResponse> &res) {
        for (const auto &middleware : middlewares)
          middleware(req, res);
        function(req, res);
      };
  RequestHandler handler(RequestType_t::GET, std::move(actualFunction),
                         urlMatch);
  this->router->AddHandler(handler);
}
void Server::Post(std::string urlTemplate,
                  const RequestHandlerFunction &function,
                  const std::vector<RequestHandlerFunction> &middlewares) {
  Url urlMatch(std::move(urlTemplate));
  RequestHandlerFunction actualFunction =
      [middlewares, function](const std::shared_ptr<HttpRequest> &req,
                              const std::shared_ptr<HttpResponse> &res) {
        for (const auto &middleware : middlewares)
          middleware(req, res);
        function(req, res);
      };
  RequestHandler handler(RequestType_t::POST, actualFunction, urlMatch);
  this->router->AddHandler(handler);
}
