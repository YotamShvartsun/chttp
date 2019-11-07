#include "chttp/data/GetRequest.h"

#include <sstream>
#include <utility>

GetRequest::GetRequest(std::vector<char> data) {
    this->FromString(std::move(data));
}

void GetRequest::FromString(std::vector<char> data) {
    std::size_t firstSpace = 0;
    std::size_t lastSpace = 0;
    std::string asString(data.begin(), data.end());
    std::string firstLine = asString.substr(0, asString.find("\r\n")); // METHOD URL PROTO
    if (firstLine.find("GET") == std::string::npos) // make sure this is a GET request
    {
        throw std::runtime_error("Request is not a valid GET request!");
    }
    // Get the url
    firstSpace = firstLine.find_first_of(' ');
    lastSpace = firstLine.find_last_of(' ') - 1;
    if (firstSpace != std::string::npos) {
        this->rawUrl = firstLine.substr(firstSpace + 1, lastSpace - firstSpace);
    } else {
#ifdef DEBUG
        std::cerr << "Request does not match the HTTP request format" << firstLine << std::endl;
#endif
        throw std::runtime_error("Request is not a valid HTTP request!");
    }
    this->BuildQuery(rawUrl);
    std::string headers = asString.substr(firstLine.length());
#ifdef DEBUG
    std::cout << "Headers: " << std::endl << headers << std::endl;
#endif
    std::vector<char> tmp(headers.begin(), headers.end());
    this->headers = HttpRequest::ParseHTTPHeaders(tmp);
}