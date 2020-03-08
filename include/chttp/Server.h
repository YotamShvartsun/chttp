#pragma once
#include <Threapool/Threadpool.h>
#include <chttp/Handler.h>
#include <chttp/Router.h>
#include <chttp/data/GetRequest.h>
#include <chttp/data/HttpRequest.h>
#include <chttp/data/HttpResponse.h>
#include <chttp/data/PostRequest.h>
#include <chttp/util/Socket.h>

class Server {
private:
  Socket server;
  ThreadPool *pool;
  Router *router;
  std::string staticFolderPath;
  std::string staticFolderUrl;
  int port = 8080;
  std::function<void(Router *, std::shared_ptr<Socket>)> ClientHandler =
      OnClient;

  static void OnClient(Router *router, std::shared_ptr<Socket> clientSocket);
  RequestHandlerFunction StaticFileHandler();

public:
  Server();
  explicit Server(Router *);

  void Run(
      int, std::function<void()> = [] {});

  void ServeStaticFolder(std::string url, std::string folderPath);
  void Get(std::string baseUrl, RequestHandlerFunction function,
           std::vector<RequestHandlerFunction> middlewares,
           std::string urlFormat = "/");
  void Post(std::string baseUrl, const RequestHandlerFunction &function,
            const std::vector<RequestHandlerFunction> &middlewares,
            std::string urlFormat = "/");

  ~Server();
};