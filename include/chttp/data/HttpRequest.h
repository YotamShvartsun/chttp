#pragma once

#include <chttp/util/Url.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class HttpRequest {
protected:
  std::string rawUrl;
  std::map<std::string, std::string> urlParams;
  std::map<std::string, std::string> query;
  std::unordered_map<std::string, std::string> headers;

  void BuildQuery(std::string);

public:
  HttpRequest();

  /**
   * @brief  Returns the base url of this request
   * @note   The url will also contain url parameters
   * @retval A string
   */
  std::string GetUrl() const;

  /**
   * @brief  Checks if param is in url parameters
   * @note   Given url: /home/dev?q=5 and Url object Url[{1, 'myname'}] url
   * params will be [{'myname':'dev'}] and query will be [{'q':5}]
   * @retval bool
   */
  bool IsInUrlParams(const std::string &) const;

  /**
   * Get the request headers
   * @return request headers
   */
  std::unordered_map<std::string, std::string> GetHeaders() const;

  /**
   * Get the URL-query part
   * @return url query
   */
  std::map<std::string, std::string> GetQuery() const;
  /**
   * @brief  Returns the value of url the parameter specified
   * @note   If the name given cannot be found, it will throw std::runtime_error
   * @retval Url parameter requested
   */
  std::string GetUrlParam(const std::string &) const;

  /**
   * @brief  Populate this->urlParams using the data given from Url object
   * @retval None
   */
  void PopulateParams(const Url &);

  /**
   * @brief  Build the request from buffer
   * @note   This is a pure-virtual method!
   * @retval None
   */
  virtual void FromString(std::vector<char>) = 0;

  /**
   * @brief Parses HTTP headers
   * @param data the header part of the request - the request until \n\r\n\r
   * @return map of the headers and the values
   */
  static std::unordered_map<std::string, std::string>
  ParseHTTPHeaders(std::vector<char> data);

  enum REQUEST_TYPES { GET, POST };
};