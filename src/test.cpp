//#define CATCH_CONFIG_MAIN
//#include <thirdparty/catch.hpp>
#include <chttp/data/PostRequest.h>
#include <chttp/data/GetRequest.h>
#include <chttp/util/Socket.h>
#include <chttp/Router.h>
#include <Threapool/Threadpool.h>
#include <chttp/data/HttpRequest.h>
#include <chttp/data/HttpResponse.h>

#include <signal.h>
#include <chttp/Handler.h>

ThreadPool *t;
Socket server;
bool IsRunning = true;

enum types {
    GET, POST
};

void ctrlCHandler(int v) {
    std::cout << "BYE";
    delete t;
    server.Close();
    exit(0);
}

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
    int type = GET;
    while (numberOfNewlines != 2) {
        lastChar = thisChar;
        thisChar = c->GetData(1)[0];
        data.push_back(thisChar);
        if (lastChar == '\r' and thisChar == '\n') {
            numberOfNewlines++;
            if (firstLine) {
                firstLine = false;
                std::string asString(data.begin(), data.end());
                if (asString.find("GET") != std::string::npos) {
                    type = GET;
                } else {
                    type = POST;
                }
            }
        } else {
            if (lastChar != '\n') {
                numberOfNewlines = 0;
            }
        }
    }
    if (POST == type) {
        std::size_t contentSize = std::stoll(HttpRequest::ParseHTTPHeaders(data)["Content-Length"]);
        std::vector<char> body = c->GetData(contentSize);
        if (contentSize != body.size())
            throw std::runtime_error("Invalid post");
        data.insert(data.end(), body.begin(), body.end());
        req.reset(new PostRequest(data));
    } else
        req.reset(new GetRequest(data));
    //c.Close();
    std::cout << "Done serving URL: " << req->GetUrl() << std::endl;
    std::shared_ptr<HttpResponse> res;
    res.reset(new HttpResponse());
    std::unordered_map<int, std::string> params;
    params[1] = "number";
    std::string base = "/plusOne";
    Url url(base, params);
    RequestHandler h(RequestType_t::GET, ff, url);
    h(req, res);
    c->SendData(res->Format());
}


int main() {
    signal(SIGINT, ctrlCHandler);
    signal(SIGTERM, ctrlCHandler);
    Router r;
    std::function<void(Router, std::shared_ptr<Socket>)> f = DoC;
    t = ThreadPool::GetInstance(&f, &r);
    server.Bind(8080);
    server.Listen();
    std::shared_ptr<Socket> c;
    int i = 0;
    while (1) {
        c = std::make_shared<Socket>(server.Accept());
        t->AddWork(c);
        i++;
    }
    t->WaitAll();
    server.Close();
    return 0;
}
