#include <chttp/data/HttpRequest.h>
#include <sstream>

HttpRequest::HttpRequest() {}

/**
 * @brief  Splitst string using one of the delimiters given in 
 * @note   this is not a part of HttpRequest, just a helper function
 * @param  data: data to split
 * @param  delimiters: optional delimiters to split by
 * @retval splitted data
 */
std::vector<std::string> SplitOptional(std::string &data, const std::string &delimiters)
{
    std::vector<std::string> result;
    std::size_t pos, prev = 0;
    while ((pos = data.find_first_of(delimiters, prev)) != std::string::npos)
    {
        if (pos > prev)
            result.push_back(data.substr(prev, pos - prev));
        prev = pos + 1;
    }
    if (prev < data.length())
        result.push_back(data.substr(prev, std::string::npos));

    return result;
}

void HttpRequest::BuildQuery(std::string url)
{
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
    fragmentPos = queryRaw.find("#");
    if (fragmentPos != std::string::npos)
    {
        queryRaw.erase(fragmentPos);
    }
// remove the fragment part from the url (its not )
#ifdef DEBUG
    std::cout << "Url raw query string: " << queryRaw << std::endl;
#endif
    querySplitted = SplitOptional(queryRaw, "&;");
    for (auto &queryPart : querySplitted)
    {
        //! note that = is not allowed as part of the query data (for example, this url is not allowed: /base/page?p1=a==)
        // ref: RFC 3986 section 3.4
        queryTmp = SplitOptional(queryPart, "=");
        if (queryTmp.size() == 1) // in case there is a url like this: proto:path?a&b=5, a=""
        {
            key = queryTmp.front();
            value = "";
        }
        else
        {
            key = queryTmp.front();
            value = queryTmp.back();
        }
        this->query.insert(std::pair<std::string, std::string>(key, value));
    }
}

bool HttpRequest::IsInUrlParams(std::string key) const
{
    return this->urlParams.find(key) != this->urlParams.end();
}

std::string HttpRequest::GetUrl() const
{
    return this->rawUrl;
}

void HttpRequest::PopulateParams(const Url& urlSpec)
{
    std::unordered_map<int, std::string> spec = urlSpec.GetUrlParamSpec();
    std::vector<std::string> splittedUrl = SplitOptional(this->rawUrl, "/");
    for(int i = 0; i < splittedUrl.size(); i++)
    {
        if(spec.find(i) != spec.end())
        {
            this->urlParams.insert(std::pair<std::string, std::string>(spec.at(i), splittedUrl[i]));
            #ifdef DEBUG
            std::cout << "Key: " << spec.at(i) << " Value: " << splittedUrl[i] << std::endl;
            #endif
        }
    }
}