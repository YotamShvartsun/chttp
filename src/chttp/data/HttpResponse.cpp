#include <iterator>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chttp/data/HttpResponse.h>


std::string HttpResponse::BuildHeaders() {
    std::string raw; // it's much easier to do this as a number of string operations
    for (auto &pair : this->headers) {
        raw += pair.first + " " + pair.second + "\r\n";
    }
    return raw;
}

HttpResponse::HttpResponse() : status(OK), dataSet(false) {

}

void HttpResponse::SetStatus(HTTP_STATUS code) {
    this->status = code;
}

void HttpResponse::Header(const std::string &key, const std::string &value) {
    this->headers.insert(std::make_pair(key, value));
}

void HttpResponse::SendFile(const std::string &path) {
    if (this->dataSet)
        throw std::logic_error("Data already set!");
    if (!std::filesystem::exists(path))
        throw std::runtime_error("File not found in path specified!");
    std::ifstream file(path, std::ios::binary);
    this->payload = std::vector<char>((std::istreambuf_iterator<char>(file)),
                                      std::istreambuf_iterator<char>());
    this->dataSet = true;

}

void HttpResponse::Raw(const std::vector<char> &data) {
    if (this->dataSet)
        throw std::logic_error("Data already set!");
    this->payload = data;
    this->dataSet = true;
}

std::vector<char> HttpResponse::Format() {
    this->Header("Content-Length", std::to_string(this->payload.size()));
    std::string responseMeta =
            std::to_string(this->status) + " " + HttpResponse::HttpCodeStrings.at(this->status) + "\r\n";
    std::vector<char> response;
    responseMeta += this->BuildHeaders();
    responseMeta += "\r\n";
    response = std::vector<char>(responseMeta.begin(), responseMeta.end());
    response.insert(response.end(), this->payload.begin(), this->payload.end());
    return response;
}
