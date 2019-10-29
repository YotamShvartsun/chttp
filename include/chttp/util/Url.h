#pragma once

#include <map>
#include <string>

class Url
{
private:
    std::string baseURL;
    std::map<int, std::string> urlParams;

public:
    Url(std::string, std::map<int, std::string>);
    /**
     * @brief  Checks if url given matches the baseURL of this instance
     * @retval bool
     */
    bool IsMatch(std::string) const;
    /**
     * @brief  Get specification for the url parmeters of this url instance
     * @note   Example: this (/home/:myname:/profile) url definition will output: [{1: 'myname'}]
     * @retval std::map<int, std::string>
     */
    std::map<int, std::string> GetUrlParamSpec() const;
};
