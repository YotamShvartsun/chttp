#include <chttp/Server.h>

void Server::OnClient(Router *router, std::shared_ptr<Socket> clientSocket) {
  std::vector<char> data;
  char thisChar = '\0', lastChar;
  size_t numberOfNewlines = 0;
  bool firstLine = true;
  std::shared_ptr<HttpRequest> req;
  RequestType_t type = RequestType_t::GET;
  while (numberOfNewlines != 2) {
    lastChar = thisChar;
    thisChar = clientSocket->GetData(1)[0];
    data.push_back(thisChar);
    if (lastChar == '\r' and thisChar == '\n') {
      numberOfNewlines++;
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
  router->Route(req, res);
  clientSocket->SendData(res->Format());
}
std::function<void(std::shared_ptr<HttpRequest>, std::shared_ptr<HttpResponse>)>
Server::StaticFileHandler() {
  return [this](const std::shared_ptr<HttpRequest> &request,
                const std::shared_ptr<HttpResponse> &response) {
    std::string requestUrl = request->GetUrl();
    std::string filePath =
        this->staticFolderPath +
        requestUrl.substr(requestUrl.find(this->staticFolderUrl) +
                          this->staticFolderUrl.size() + 1);
    response->SendFile(filePath);
  };
}
Server::Server() {
  std::vector<RequestHandler> defaultHandlers;
  this->router = Router::GetInstance(defaultHandlers);
  this->pool = ThreadPool::GetInstance(&this->ClientHandler, this->router);
}
Server::Server(Router *router) {
  this->router = router;
  this->pool = ThreadPool::GetInstance(&this->ClientHandler, this->router);
}
void Server::Run(int port = 8080, std::function<void()> onStart) {
  std::shared_ptr<Socket> client;
  this->port = port;
  this->server.Bind(this->port);
  this->server.Listen();
  onStart();
  while (true) {
    client = std::make_shared<Socket>(this->server.Accept());
    this->pool->AddWork(client);
  }
}
void Server::ServeStaticFolder(std::string url, std::string folderPath) {
  this->staticFolderUrl = url;
  this->staticFolderPath = folderPath;
  std::unordered_map<int, std::string> matchAll;
  matchAll[0] = "*";
  Url urlMatch(this->staticFolderUrl, matchAll);
  RequestHandler handler(RequestType_t::GET, this->StaticFileHandler(),
                         urlMatch);
  this->router->AddHandler(handler);
}
void Server::Get(std::string baseUrl, RequestHandlerFunction function,
                 std::vector<RequestHandlerFunction> middlewares,
                 std::string urlFormat) {
  auto urlFormatParser = RequestHandler::CreateParamMap(std::move(urlFormat));
  Url urlMatch(std::move(baseUrl), urlFormatParser);
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
void Server::Post(std::string baseUrl, const RequestHandlerFunction &function,
                  const std::vector<RequestHandlerFunction> &middlewares,
                  std::string urlFormat) {
  auto urlFormatParser = RequestHandler::CreateParamMap(std::move(urlFormat));
  Url urlMatch(std::move(baseUrl), urlFormatParser);
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
Server::~Server() {
  this->server.Close();
  delete this->pool;
  delete this->router;
  ThreadPool::Reset();
  Router::Reset();
}
