#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <chttp/util/Url.h>

class HttpRequest
{
private:
    std::string rawUrl;
    std::map<std::string, std::string> urlParams;
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
     * @note   Given url: /home/dev?q=5 and Url object Url[{1, 'myname'}] url params will be [{'myname':'dev'}] and query will be [{'q':5}]
     * @retval bool
     */
    bool IsInUrlParams(std::string) const;
    /**
     * @brief  Returns the value of url the parameter specified
     * @note   If the name given cannot be found, it will throw std::runtime_error
     * @retval Url parameter requested
     */
    std::string GetUrlParam(std::string) const;
    /**
     * @brief  Populate this->urlParams using the data given from Url object
     * @retval None
     */
    void PopulateParams(const Url&);
    /**
     * @brief  Build the request from buffer
     * @note   This is a pure-virtual method!
     * @retval None
     */
    virtual void FromString(std::vector<char>) = 0;
};