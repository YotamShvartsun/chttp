//#define CATCH_CONFIG_MAIN
//#include <thirdparty/catch.hpp>
#include <chttp/data/PostRequest.h>
#include <chttp/data/GetRequest.h>
#include <chttp/util/Socket.h>
#include <chttp/Router.h>
#include <Threapool/Threadpool.h>
#include <chttp/data/HttpRequest.h>


enum types {
    GET, POST
};

void DoC(Router r, Socket *c) {
    std::vector<char> data;
    char thisChar = '\0', lastChar;
    size_t numberOfNewlines = 0;
    bool firstLine = true;
    HttpRequest *req;
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
        req = new PostRequest(data);
    } else
        req = new GetRequest(data);
    //c.Close();
    std::cout << "Done serving URL: " << req->GetUrl() << std::endl;
}


int main() {
    Router r;
    std::function<void(Router, Socket *)> f = DoC;
    ThreadPool *t = ThreadPool::GetInstance(&f, &r);
    Socket server;
    server.Bind(8080);
    server.Listen();
    Socket c = server.Accept();
    t->AddWork(&c);
    t->WaitAll();
    Socket c1 = server.Accept();
    t->AddWork(&c1);
    t->WaitAll();
    return 0;
}
>>>>>>> Stashed changes
