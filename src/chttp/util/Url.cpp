#include <chttp/util/Url.h>
#include <string>

Url::Url(std::string& baseUrl, std::unordered_map<int, std::string> &spec)
{
    this->baseURL = baseUrl;
    this->urlParams = spec;
}

bool Url::IsMatch(std::string url) const
{
    size_t beginBase, endBase;
    std::string baseURL;
    beginBase = url.find_first_not_of('/'); // first char is always /
    if(beginBase == std::string::npos)
        return false;
    endBase = url.find_first_of('/', beginBase);
    baseURL = url.substr(beginBase, endBase - 1);
    return baseURL == this->baseURL;
}

std::unordered_map<int, std::string> Url::GetUrlParamSpec() const
{
    return this->urlParams;
}