/**
 * @file
 * @author Yotam Shvartsun <yotam.shvartsun@gmail.com>
 * @version 1.0
 * @section DESCRIPTION
 * This file contains the definitions for the Server class, which is the main
 * class of CHttp
 */
#pragma once
#include <chttp/RequestHandler.h>
#include <chttp/Router.h>
#include <chttp/Threapool/Threadpool.h>
#include <chttp/data/GetRequest.h>
#include <chttp/data/HttpRequest.h>
#include <chttp/data/HttpResponse.h>
#include <chttp/data/PostRequest.h>
#include <chttp/util/Socket.h>

class Server {
private:
  /**
   * The listener socket
   */
  Socket server;
  /**
   * The task executor, which executes OnClient for each client socket
   */
  Threadpool *pool;
  /**
   * The router, used to route requests
   */
  Router *router;
  /**
   * The path of the static-files folder
   */
  std::string staticFolderPath;
  /**
   * The base-url of the static files
   */
  std::string staticFolderUrl;
  /**
   * The exposed port
   */
  int port = 8080;
  /**
   * The client handler holder
   */
  std::function<void(Router *, std::shared_ptr<Socket>)> ClientHandler =
      OnClient;
  /**
   * This function handles each client
   * @param router The router used to route the clients and to serve them
   * @param clientSocket The client
   */
  static void OnClient(Router *router,
                       const std::shared_ptr<Socket> &clientSocket);
  /**
   * Creates the static file handler as a server-independent function
   * @return An handler used to serve static files
   */
  RequestHandlerFunction StaticFileHandler();

public:
  /**
   * Crete a new server
   */
  Server();

  /**
   * Starts the server
   * @param port The port to listen on
   * @param OnStart Function to run when the server starts
   */
  void Run(
      int port, const std::function<void()> OnStart = [] {});
  /**
   * Add a static folder
   * @param url Url of the static files
   * @param folderPath The path of the folder
   * @note ``folderPath`` can be a relative path as well
   */
  void ServeStaticFolder(std::string url, std::string folderPath);
  /**
   * Add a GET route with middlewares
   * @param baseUrl The base-Url of the route
   * @param function The user defined handler
   * @param middlewares A list of functions that should run before the handler
   * runs
   * @param urlFormat A url format used to create the url-template
   */
  void Get(std::string baseUrl, const RequestHandlerFunction& function,
           const std::vector<RequestHandlerFunction>& middlewares,
           std::string urlFormat = "/");
  /**
   * Add a POST route with middlewares
   * @param baseUrl The base-Url of the route
   * @param function The user defined handler
   * @param middlewares A list of functions that should run before the handler
   * runs
   * @param urlFormat A url format used to create the url-template
   */
  void Post(std::string baseUrl, const RequestHandlerFunction &function,
            const std::vector<RequestHandlerFunction> &middlewares,
            std::string urlFormat = "/");
  /**
   * Stop the server and release all resources acquired
   */
  ~Server();
};