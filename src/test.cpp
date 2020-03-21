#include <chttp/CHttp.h>
#include <csignal>
#include <iostream>

// void ctrlCHandler(int v) {
//  std::cout << "BYE";
//  delete t;
//  server.Close();
//  exit(0);
//}

void ff(std::shared_ptr<HttpRequest> rq, std::shared_ptr<HttpResponse> rs) {
  std::string aa = rq->GetUrlParam("number");
  int ttt = std::stoi(aa);
  std::string ress = std::to_string(ttt + 1);
  rs->Raw(std::vector<char>(ress.begin(), ress.end()));
}

void DoC(Router r, std::shared_ptr<Socket> c) {
  std::vector<char> data;
  char thisChar = '\0', lastChar;
  size_t numberOfNewlines = 0;
  bool firstLine = true;
  std::shared_ptr<HttpRequest> req;
  RequestType_t type = RequestType_t::GET;
  while (numberOfNewlines != 2) {
    lastChar = thisChar;
    thisChar = c->GetData(1)[0];
    data.push_back(thisChar);
    if (lastChar == '\r' && thisChar == '\n') {
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
    std::vector<char> body = c->GetData(contentSize);
    if (contentSize != body.size())
      throw std::runtime_error("Invalid post");
    data.insert(data.end(), body.begin(), body.end());
    req.reset(new PostRequest(data));
  } else
    req.reset(new GetRequest(data));
  std::shared_ptr<HttpResponse> res;
  res.reset(new HttpResponse());
  r.Route(req, res);
  c->SendData(res->Format());
}

void fff(std::shared_ptr<HttpRequest> rq, std::shared_ptr<HttpResponse> rs) {
  rs->SetStatus(HTTP_STATUS::OK);
}

void f(std::shared_ptr<HttpRequest> rq, std::shared_ptr<HttpResponse> rs) {
  auto b = ((PostRequest *)rq.get())->GetBody();
  auto ct = rq->GetHeaders()["Content-Type"];
  MultipartField mp(b, ct);
  auto be = b.begin() + mp.getData().size();
  std::vector<char> vv(be, b.end());
  MultipartField pp(vv, ct);
  rs->Raw(mp.getData());
}

void printURL(std::shared_ptr<HttpRequest> rq,
              std::shared_ptr<HttpResponse> rs) {
  std::cout << rq->GetUrl() << std::endl;
}

int main() {
  //    signal(SIGINT, ctrlCHandler);
  //    signal(SIGTERM, ctrlCHandler);
  //    std::vector<RequestHandler> routes;
  //    auto x = RequestHandler::CreateParamMap("/api/plusOne/:number");
  //    Url url("/", x);
  //    RequestHandler h(RequestType_t::GET, ff, url);
  //    routes.push_back(h);
  //    Router *r = Router::GetInstance(routes);
  //    std::function<void(Router, std::shared_ptr<Socket>)> f = DoC;
  //    t = ThreadPool::GetInstance(&f, r);
  //    server.Bind(8080);
  //    server.Listen();
  //    std::shared_ptr<Socket> c;
  //    while (1) {
  //        c = std::make_shared<Socket>(server.Accept());
  //        t->AddWork(c);
  //    }
  //    t->WaitAll();
  //    server.Close();
  //    return 0;
  Server server;
  std::vector<RequestHandlerFunction> v{printURL};
  server.Get("/", fff, v, "/");
  server.Get("/", ff, {}, "/:number");
  server.Post("/", f, {});
  server.ServeStaticFolder("/static", "/home/themiper/");
  server.Run(8080, [] { std::cout << "Server up" << std::endl; });
  return 0;
}
