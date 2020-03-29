/**
 * @file
 * @author Yotam Shvartsun <yotam.shvartsun@gmail.com>
 * @version 1.0
 * @section DESCRIPTION
 * This file contains the definition of the Url class, which represents a URL
 * template, needed in order to parse URL-parameters and to check url matches
 */
#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

/**
 * This class represents a URL template, used to parse URL-passed-parameters and
 * to check URL matches
 */
class Url {
private:
  //  std::string baseURL;
  std::unordered_map<int, std::string> mustBe;
  std::unordered_map<int, std::string> parameters;

public:
  Url(std::string baseUrl, std::unordered_map<int, std::string> &spec);
  Url(std::string urlTemplate);
  /**
   * @brief  Checks if url given matches the baseURL of this instance
   * @retval bool
   */
  bool IsMatch(const std::string &) const;

  /**
   * @brief  Get specification for the url parameters of this url instance
   * @note   Example: this (/home/:myname:/profile) url definition will output:
   * {1: 'myname'}
   * @retval std::unordered_map<int, std::string>
   */
  std::unordered_map<int, std::string> GetUrlParamSpec() const;

  ~Url() = default;
};
