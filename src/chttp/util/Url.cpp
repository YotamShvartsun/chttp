#include <chttp/util/Url.h>
#include <string>

Url::Url(std::string baseUrl, std::map<int, std::string> spec) : baseURL(baseUrl), urlParams(spec)
{
}

bool Url::IsMatch(std::string url) const
{
    return url.find(this->baseURL) != std::string::npos;
}

std::map<int, std::string> Url::GetUrlParamSpec() const
{
    return this->urlParams;
}