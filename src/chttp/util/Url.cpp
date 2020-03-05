#include <chttp/util/Url.h>
#include <string>

Url::Url(std::string &baseUrl, std::unordered_map<int, std::string> &spec) {
    this->baseURL = baseUrl;
    this->urlParams = spec;
}

bool Url::IsMatch(const std::string &url) const {
    return url.find(this->baseURL) != std::string::npos;
}

std::unordered_map<int, std::string> Url::GetUrlParamSpec() const {
    return this->urlParams;
}