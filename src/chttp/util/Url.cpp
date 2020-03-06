#include <algorithm>
#include <chttp/data/HttpRequest.h>
#include <chttp/util/Url.h>
#include <string>
#include <utility>

Url::Url(std::string baseUrl, std::unordered_map<int, std::string> &spec) {
  this->baseURL = std::move(baseUrl);
  this->urlParams = spec;
}

bool Url::IsMatch(const std::string &url) const {
  if (url.find(this->baseURL) == std::string::npos)
    return false;
  std::string afterBase = url.substr(url.find(this->baseURL) + 1);
  if (afterBase.back() != '/')
    afterBase += '/';
  if (this->urlParams.empty() && afterBase == "/")
    return true;
  return std::count(afterBase.begin(), afterBase.end(), '/') ==
         this->urlParams.size();
}

std::unordered_map<int, std::string> Url::GetUrlParamSpec() const {
  return this->urlParams;
}