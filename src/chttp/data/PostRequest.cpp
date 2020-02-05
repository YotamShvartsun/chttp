#include <vector>
#include <chttp/data/PostRequest.h>
#include <chttp/util/MultipartField.h>
#include <algorithm>

void PostRequest::FromString(std::vector<char> data) {
    std::size_t firstSpace = 0;
    std::size_t lastSpace = 0;
    std::string asString(data.begin(), data.end());
    std::string firstLine = asString.substr(0, asString.find("\r\n")); // METHOD URL PROTO
    if (firstLine.find("POST") == std::string::npos) // make sure this is a GET request
    {
        throw std::runtime_error("Request is not a valid POST request!");
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
    std::size_t headersEnd = asString.find("\r\n\r\n");
    std::string headers = asString.substr(firstLine.length(), headersEnd - firstLine.length());
#ifdef DEBUG
    std::cout << "Headers: " << std::endl << headers << std::endl;
#endif
    std::vector<char> tmp(headers.begin(), headers.end());
    this->headers = HttpRequest::ParseHTTPHeaders(tmp);
    try {
        long bodySize = std::stol(this->headers.at("Content-Length"));
        std::string body = asString.substr(headersEnd + 4, bodySize);
        this->body = std::vector<char>(body.begin(), body.end());
    }catch (std::exception& e)
    {
#ifdef DEBUG
        std::cerr << "POST: " << e.what() << std::endl;
#endif
    }
#ifdef DEBUG
    for(auto x: this->body)
    {
        std::cout<<x;
    }
#endif
}

PostRequest &PostRequest::operator=(const PostRequest & other) {
    std::copy(other.rawUrl.begin(), other.rawUrl.end(), this->rawUrl.begin());
    this->query = other.query;
    this->headers = other.headers;
    this->urlParams = other.urlParams;
    return *this;
}

PostRequest::PostRequest(std::vector<char> data) {
    this->FromString(data);
    try {
        if (this->headers.at("Content-Type").find("multipart") != std::string::npos)
        {
            std::string bodyString(this->body.begin(), this->body.end());
            std::size_t lastNewline = 0;
            std::string contentType = this->headers.at("Content-Type");
            std::string boundary = contentType.substr(contentType.find(';') + 1);
            boundary = boundary.substr(boundary.find("boundary=") + 9);
            boundary = "--" + boundary;

        }
    } catch (std::exception &e) {
        this->headers["Content-Type"] = "raw";
    }
}

std::vector<char> PostRequest::GetBody() const {
    return this->body;
}
