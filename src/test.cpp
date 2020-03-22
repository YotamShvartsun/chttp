#include <chttp/CHttp.h>
#include <iostream>

void form(std::shared_ptr<HttpRequest> rq, std::shared_ptr<HttpResponse> rs) {
  auto b = ((PostRequest *)rq.get())->GetBody();
  auto ct = rq->GetHeaders()["Content-Type"];
  MultipartField mp(b, ct);
  auto data = mp.getData();
  std::string res = std::string(data.begin(), data.end()) + " to you too";
  rs->Raw(std::vector<char>(res.begin(), res.end()));
}
int main(int argc, char **argv) {
  int port = 8080;
  Server app;
  app.Post("/form", form, {});
  app.ServeStaticFolder("/", "../examples/public/");
  app.Run(port);
}