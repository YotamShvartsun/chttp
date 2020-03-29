#include <algorithm>
#include <chttp/data/HttpRequest.h>
#include <chttp/util/Url.h>
#include <sstream>
#include <string>
#include <utility>


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
      if (item != ":*") {
        this->parameters[i] = item.substr(1);
      } else {
        this->parameters[-1] = "*";
      }
    } else {
      if (item != "") {
        this->mustBe[i] = item;
      }
    }
    i++;
  }
}
bool Url::IsMatch(const std::string &url) const {
  std::string urlTmp = url.substr(1);
  if (urlTmp.empty()) {
    return this->mustBe.empty() && this->parameters.empty();
  }
  bool checkOnlyBase = this->parameters.find(-1) != this->parameters.end();
  std::stringstream asStream(urlTmp);
  std::string part;
  int idx = 0;
  int paramsToEnter = this->parameters.size();
  while (std::getline(asStream, part, '/')) {
    if (!checkOnlyBase &&
        this->parameters.find(idx) != this->parameters.end()) {
      paramsToEnter--;
    } else {
      if (this->mustBe.find(idx) != this->mustBe.end()) {
        if (this->mustBe.at(idx) != part) {
          return false; // this means that the url doesnt fit the template, it
                        // must fit to the given value!
        }
      } else {
        if (!checkOnlyBase) {
          return false; // The url is longer then the template!
        }
      }
    }
    idx++;
  }
  // If all parameters were given value, the url is valid!
  return checkOnlyBase || paramsToEnter == 0;
}
