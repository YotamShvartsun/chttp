/**
 * @file
 * @author Yotam Shvartsun <yotam.shvartsun@gmail.com>
 * @version 1.0
 * @section DESCRIPTION
 * This file contains the definition of HttpRequest - the base class
 * representing a valid Http request sent to the server. This is the base class
 * of CHttp, and all HTTP-request objects are implementing it
 */
#pragma once

#include <chttp/util/Url.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

/**
 * This class represents a valid HTTP request
 * @note This class contains a virtual method, `FromString`. Every class
 * inheriting from this class <b>must</b> implement it
 */
class HttpRequest {
protected:
  /**
   * The url of the request, as is.
   */
  std::string rawUrl;
  /**
   * User defined url parameters and their values
   * Url parameters are stored as a part of the url. Url parameters can be
   * parsed using a template, defined by a base url and a value map.
   * @note The `Url` class stores the parameter map, and
   * ``HttpRequest::PopulateParams`` populates this map
   */
  std::map<std::string, std::string> urlParams;
  /**
   * The query items, parsed by ``HttpRequest::BuildQuery``
   */
  std::map<std::string, std::string> query;
  /**
   * The request headers
   */
  std::unordered_map<std::string, std::string> headers;
  /**
   * Stores additional items parsed by middlewares and other functions
   */
  std::unordered_map<std::string, std::string> additionalData;

  /**
   * Parses urls query string
   * @related to RFC 3986
   * @param url The url as it appears in the request
   * @result The query data will be stored in ``HttpRequest::query``
   */
  void BuildQuery(std::string);

public:
  /**
   * Build an empty request
   */
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
   * @return Url parameter requested
   */
  std::string GetUrlParam(const std::string &) const;

  /**
   * @brief  Populate this->parameters using the data given from Url object
   * @retval None
   */
  void PopulateParams(const Url &);

  /**
   * Add a new item to the additional-data dictionary, created by middlewares
   * @param key The name of the item
   * @param value Value to set
   */
  void AddAdditional(std::string key, std::string value);
  /**
   * Retrive an item from the additional-data dictionary
   * @param key Name of the item
   * @return Value
   */
  std::string GetAdditional(const std::string &key);

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

  /**
   * This enum represents the different request types possible. For now, you can only use POST and GET.
   * When extending this framework, add request types to this enum as well.
   */
  enum REQUEST_TYPES { GET, POST };
};