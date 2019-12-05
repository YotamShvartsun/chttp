#include <iterator>
#include <iostream>
#include <fstream>
// including headers in order to stop using the std::filesystem lib, due to bug in libc++
// this bug IS solved in gcc-9 and g++-9, but this should work with other compiler versions too
#ifndef _WIN32

#include <unistd.h>

#else
#include <io.h>
#include <Windows.h>
#define access access_s
#endif

#include <chttp/data/HttpResponse.h>


/**
 * Check if file in path exists
 * @param path path to check
 * @return bool
 */
bool FileExists(const std::string &path) {
    return access(path.c_str(), 0) == 0;
}

std::string HttpResponse::BuildHeaders() {
    std::string raw; // it's much easier to do this as a number of string operations
    for (auto &pair : this->headers) {
        raw += pair.first + ": " + pair.second + "\r\n";
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
    if (!FileExists(path))
        throw std::runtime_error("MultipartField not found in path specified!");
    std::string ext = path.substr(path.find_last_of('.'));
    std::ifstream file(path, std::ios::binary);
    this->payload = std::vector<char>((std::istreambuf_iterator<char>(file)),
                                      std::istreambuf_iterator<char>());
    this->Header("Content-type", HttpResponse::MIME_TYPES.at(ext));
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
            "HTTP/1.1" + std::to_string(this->status) + " " + HttpResponse::HttpCodeStrings.at(this->status) + "\r\n";
    std::vector<char> response;
    responseMeta += this->BuildHeaders();
    responseMeta += "\r\n";
    response = std::vector<char>(responseMeta.begin(), responseMeta.end());
    response.insert(response.end(), this->payload.begin(), this->payload.end());
    response.push_back('\r');
    response.push_back('\n');
    response.push_back('\r');
    response.push_back('\n');
    return response;
}
