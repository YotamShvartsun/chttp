#include <algorithm>
#include <chttp/data/HttpRequest.h>
#include <chttp/util/Url.h>
#include <sstream>
#include <string>
#include <utility>

Url::Url(std::string baseUrl, std::unordered_map<int, std::string> &spec) {
  this->baseURL = std::move(baseUrl);
  this->parameters = spec;
}

bool Url::IsMatch(const std::string &url) const {
  if (url.find(this->baseURL) == std::string::npos)
    return false;
  if (url == this->baseURL && this->parameters.empty())
    return true;
  std::string afterBase = url.substr(url.find(this->baseURL) + 1);
  if (afterBase.empty() || afterBase.back() != '/')
    afterBase += '/';
  if (this->parameters.empty() && afterBase == "/")
    return true;
  if (this->parameters.find(0) != this->parameters.end()) {
    if (this->parameters.at(0) == "*") {
      return true;
    }
  }
  return std::count(afterBase.begin(), afterBase.end(), '/') ==
         this->parameters.size();
}

std::unordered_map<int, std::string> Url::GetUrlParamSpec() const {
  return this->parameters;
}
Url::Url(std::string urlTemplate) {
  urlTemplate = urlTemplate.substr(1);
  std::istringstream iss(urlTemplate);
  std::string item;
  int i = 0;
  while (std::getline(iss, item, '/')) {
    if (item[0] == ':') {
      this->parameters[i] = item.substr(1);
    }
    else {
      this->mustBe[i] = item;
    }
    i++;
  }
}
