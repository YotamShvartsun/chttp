#include <chttp/Handler.h>

#include <utility>
#include <chttp/data/GetRequest.h>
#include <chttp/data/PostRequest.h>

RequestHandler::RequestHandler(RequestType_t type,
                               std::function<void(std::shared_ptr<HttpRequest>, std::shared_ptr<HttpResponse>)> handler,
                               const Url &urlMatch) : urlMatch(urlMatch) {
    this->requestType = type;
    this->handler = std::move(handler);
}

bool RequestHandler::IsMatching(const std::shared_ptr<HttpRequest> &request) const {
    if (dynamic_cast<GetRequest *>(request.get()))
        if (this->requestType != RequestType_t::GET)
            return false;
    if (dynamic_cast<PostRequest *>(request.get()))
        if (this->requestType != RequestType_t::POST)
            return false;
    return this->urlMatch.IsMatch(request->GetUrl());
}

void RequestHandler::operator()(const std::shared_ptr<HttpRequest> &request, std::shared_ptr<HttpResponse> response) {
    if (this->IsMatching(request)) {
        request->PopulateParams(this->urlMatch);
        this->handler(request, std::move(response));
    }
}

