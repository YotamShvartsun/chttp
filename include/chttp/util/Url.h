#pragma once

#include <iostream>
#include <unordered_map>
#include <string>

class Url {
private:
    std::string baseURL;
    std::unordered_map<int, std::string> urlParams;

public:
  Url(std::string baseUrl, std::unordered_map<int, std::string> &spec);

  /**
   * @brief  Checks if url given matches the baseURL of this instance
   * @retval bool
   */
  bool IsMatch(const std::string &) const;

    /**
     * @brief  Get specification for the url parmeters of this url instance
     * @note   Example: this (/home/:myname:/profile) url definition will output: [{1: 'myname'}]
     * @retval std::unordered_map<int, std::string>
     */
    std::unordered_map<int, std::string> GetUrlParamSpec() const;

    ~Url() = default;
};
