#include <chttp/util/Url.h>
#include <string>

Url::Url(std::string& baseUrl, std::unordered_map<int, std::string> &spec)
{
    this->baseURL = baseUrl;
    this->urlParams = spec;
}

bool Url::IsMatch(const std::string &url) const {
    size_t beginBase, endBase;
    std::string maybeBaseURL;
    beginBase = url.find_first_not_of('/'); // first char is always /
    if (beginBase == std::string::npos)
        return false;
    endBase = url.find_first_of('/', beginBase);
    maybeBaseURL = url.substr(beginBase - 1, endBase);
    return maybeBaseURL == this->baseURL;
}

std::unordered_map<int, std::string> Url::GetUrlParamSpec() const
{
    return this->urlParams;
}