#include <chttp/data/HttpRequest.h>
#include <sstream>

HttpRequest::HttpRequest() = default;

/**
 * splits data to two parts: everything before del and everything after it
 * @throws error when data doesn't contain del
 * @param data
 * @param del
 * @return vector
 */
std::vector<std::string> SplitOnce(std::string &data, const std::string &del) {
    std::string tmp = data.substr(0, data.find(del, 0));
    std::vector<std::string> res;
    res.push_back(tmp);
    res.push_back(data.substr(tmp.length() + 1));
    return res;
}

/**
 * @brief  Splits string using one of the delimiters given in
 * @note   this is not a part of HttpRequest, just a helper function
 * @param  data: data to split
 * @param  delimiters: optional delimiters to split by
 * @retval Splitted data
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
    fragmentPos = queryRaw.find('#');
    if (fragmentPos != std::string::npos)
    {
        queryRaw.erase(fragmentPos);
    }
// remove the fragment part from the url (its not )
#ifdef DEBUG
    std::cout << "Url raw query string: " << queryRaw << std::endl;
#endif
    querySplitted = SplitOptional(queryRaw, "&;");
    if (querySplitted.size() == 1 && querySplitted[0] == queryRaw)
        return;
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

bool HttpRequest::IsInUrlParams(const std::string &key) const
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
    if(!urlSpec.IsMatch(this->rawUrl))
        throw std::runtime_error("Url given dose not match to url spec");
    for (size_t i = 0; i < splittedUrl.size(); i++)
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

std::unordered_map<std::string, std::string> HttpRequest::ParseHTTPHeaders(std::vector<char> data) {
    std::string asString(data.data());
    std::vector<std::string> lines, filteredLines;
    std::unordered_map<std::string, std::string> res;
    std::size_t prev = 0, pos;
    while ((pos = asString.find("\r\n", prev)) != std::string::npos) {
        if (asString[prev] == '\n') {
            prev++;
        }
        if (pos > prev)
            lines.push_back(asString.substr(prev, pos - prev));
        prev = pos + 1;
    }
    if (prev < asString.length())
        lines.push_back(asString.substr(prev, std::string::npos));
#ifdef DEBUG
    for (auto &l : lines)
        std::cout << l << std::endl;
#endif
    // filter the lines to the headers part (contains ':')
    for (auto &l : lines) {
        if (l == "\n")
            continue; // \n means this is the last line
        if (l.find(':') != std::string::npos)
            filteredLines.push_back(l);
    }

#ifdef DEBUG
    std::cout << "Filtered lines:" << std::endl;
    for (auto &l : filteredLines)
        std::cout << l << std::endl;
#endif
    std::vector<std::string> tmp;
    for (auto &l : filteredLines) {
        tmp = SplitOnce(l, ":");
        res.insert(std::make_pair(tmp.front(), tmp.back()));
    }
#ifdef DEBUG
    std::cout << "RESULT:" << std::endl;
    for (auto &i : res)
        std::cout << i.first << ":" << i.second << std::endl;
#endif
    return res;
}

std::unordered_map<std::string, std::string> HttpRequest::GetHeaders() const {
    return this->headers;
}

std::map<std::string, std::string> HttpRequest::GetQuery() const {
    return this->query;
}

std::string HttpRequest::GetUrlParam(const std::string& name) const {
    if(!this->IsInUrlParams(name))
        throw std::runtime_error("No param named " + name);
    return this->urlParams.at(name);
}
