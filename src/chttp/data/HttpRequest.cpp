/**
 * @file
 * @author Yotam Shvartsun <yotam.shvartsun@gmail.com>
 * @version 1.0
 * @section DESCRIPTION
 * This file contains the implementation of the HttpRequest non-virtual
 * functions In this file there are also helper functions used by the CHttp
 * library, in order to preform repetetive string relaeted actions
 */
#include <chttp/data/HttpRequest.h>
#include <sstream>
#include <utility>

HttpRequest::HttpRequest() = default;

/**
 * Splits data to two parts: everything before del and everything after it
 * @throws std::out_of_range when data doesn't contain del
 * @param data string to split
 * @param del delimiter to split by
 * @return The splitted string
 */
std::vector<std::string> SplitOnce(std::string &data, const std::string &del) {
  std::string tmp = data.substr(0, data.find(del, 0));
  std::vector<std::string> res;
  res.push_back(tmp);
  res.push_back(data.substr(tmp.length() + 1));
  return res;
}

/**
 * @brief  Splits string using one of the delimiters given in
 * @param  data: data to split
 * @param  delimiters: optional delimiters to split by
 * @return Splitted data
 */
std::vector<std::string> SplitOptional(std::string &data,
                                       const std::string &delimiters) {
  std::vector<std::string> result;
  std::size_t pos, prev = 0;
  while ((pos = data.find_first_of(delimiters, prev)) != std::string::npos) {
    if (pos > prev)
      result.push_back(data.substr(prev, pos - prev));
    prev = pos + 1;
  }
  if (prev < data.length())
    result.push_back(data.substr(prev, std::string::npos));

  return result;
}

void HttpRequest::BuildQuery(std::string url) {
#ifdef DEBUG
  std::cout << "In BuildQuery" << std::endl
            << "this: " << this << std::endl
            << "url: " << url << std::endl;
#endif
  std::string queryRaw;
  std::vector<std::string> querySplitted;
  std::vector<std::string> urlSplitted;
  std::string tmp;
  std::size_t fragmentPos;
  std::vector<std::string> queryTmp;
  std::string key, value;

  urlSplitted = SplitOptional(url, "?");
  queryRaw = urlSplitted.back();
  // Remove the fragment from the url
  fragmentPos = queryRaw.find('#');
  if (fragmentPos != std::string::npos) {
    queryRaw.erase(fragmentPos);
  }
#ifdef DEBUG
  std::cout << "Url raw query string: " << queryRaw << std::endl;
#endif
  // split the query using the delimiters of a query string
  //! @note that the query string is defined as everything between the ? to a #
  //! or the end of the url
  //! @see <a href="https://tools.ietf.org/html/rfc3986#section-3.4">RFC 3986,
  //! section 3.4</a>
  querySplitted = SplitOptional(queryRaw, "&;");
  if (querySplitted.size() == 1 && querySplitted[0] == queryRaw)
    return;

  for (auto &queryPart : querySplitted) {
    //! note that = is not allowed as part of the query data (for example, this
    //! url is not allowed: /base/page?p1=a==)
    // ref: RFC 3986 section 3.4
    queryTmp = SplitOptional(queryPart, "=");
    if (queryTmp.size() ==
        1) // in case there is a url like this: proto:path?a&b=5, a=""
    {
      key = queryTmp.front();
      value = "";
    } else {
      key = queryTmp.front();
      value = queryTmp.back();
    }
    this->query.insert(std::pair<std::string, std::string>(key, value));
  }
}

bool HttpRequest::IsInUrlParams(const std::string &key) const {
  return this->urlParams.find(key) != this->urlParams.end();
}

std::string HttpRequest::GetUrl() const { return this->rawUrl; }

void HttpRequest::PopulateParams(const Url &urlSpec) {
  // If the url dose not match the template, throw an error and stop
  if (!urlSpec.IsMatch(this->rawUrl))
    throw std::runtime_error("Url given dose not match to url spec");

  std::unordered_map<int, std::string> spec = urlSpec.GetUrlParamSpec();
  std::vector<std::string> splittedUrl = SplitOptional(this->rawUrl, "/");
  for (size_t i = 0; i < splittedUrl.size(); i++) {
    if (spec.find(i) != spec.end()) {
      this->urlParams.insert(
          std::pair<std::string, std::string>(spec.at(i), splittedUrl[i]));
#ifdef DEBUG
      std::cout << "Key: " << spec.at(i) << " Value: " << splittedUrl[i]
                << std::endl;
#endif
    }
  }
}

std::unordered_map<std::string, std::string>
HttpRequest::ParseHTTPHeaders(std::vector<char> data) {
  std::string asString(data.begin(), data.end());
  std::vector<std::string> lines, filteredLines;
  std::unordered_map<std::string, std::string> res;
  std::size_t prev = 0, pos;
  // Split the request to lines
  while ((pos = asString.find("\r\n", prev)) != std::string::npos) {
    if (asString[prev] == '\n') {
      prev++;
    }
    if (pos > prev)
      lines.push_back(asString.substr(prev, pos - prev));
    prev = pos + 1;
  }
  // also include the last line
  if (prev < asString.length())
    lines.push_back(asString.substr(prev, std::string::npos));
#ifdef DEBUG
  for (auto &l : lines)
    std::cout << l << std::endl;
#endif
  // filter the lines to the headers part (contains ':')
  for (auto &l : lines) {
    if (l == "\n")
      continue; // \n means this is the last line
    if (l.find(':') != std::string::npos)
      filteredLines.push_back(l);
  }

#ifdef DEBUG
  std::cout << "Filtered lines:" << std::endl;
  for (auto &l : filteredLines)
    std::cout << l << std::endl;
#endif
  std::vector<std::string> tmp;
  for (auto &l : filteredLines) {
    /**
     * A valid HTTP header must have a colon in it, which seperates between the
     * field name and the value
     * @see <a href="https://tools.ietf.org/html/rfc2616#section-4.2">RFC 2616,
     * section 4.2</a>
     */
    tmp = SplitOnce(l, ":");
    res.insert(std::make_pair(tmp.front(), tmp.back()));
  }
#ifdef DEBUG
  std::cout << "RESULT:" << std::endl;
  for (auto &i : res)
    std::cout << i.first << ":" << i.second << std::endl;
#endif
  return res;
}

std::unordered_map<std::string, std::string> HttpRequest::GetHeaders() const {
  return this->headers;
}

std::map<std::string, std::string> HttpRequest::GetQuery() const {
  return this->query;
}

std::string HttpRequest::GetUrlParam(const std::string &name) const {
  if (!this->IsInUrlParams(name)) // if the parameter is not in the list, throw
                                  // runtime error instead of the out_of_range
    throw std::runtime_error("Parameter named " + name +
                             " was not found in the url");
  return this->urlParams.at(name);
}
void HttpRequest::AddAdditional(std::string key, std::string value) {
#ifdef DEBUG
  if (this->additionalData.find(key) != this->additionalData.end()) {
    std::cout << "Key named " << key << " alredy exists, overwriting"
              << std::endl;
  }
#endif
  this->additionalData[key] = std::move(value);
}
std::string HttpRequest::GetAdditional(const std::string &key) {
  return this->additionalData[key];
}
