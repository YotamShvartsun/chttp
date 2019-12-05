#include <iostream>
#include "chttp/util/Url.h"
#include "chttp/data/HttpRequest.h"
#include "chttp/data/GetRequest.h"
#include "thirdparty/catch.hpp"
#define FAIL return false;
TEST_CASE("GetRequest can contain headers", "[GetRequest]")
{
    std::string tmp("GET / HTTP/1.1\r\nHeader:Value\r\nCookie:yo=aaaXz55==;data=rrr\r\n\r\n");
    std::vector<char> req(tmp.begin(), tmp.end());
    std::unordered_map<std::string, std::string> res;
    SECTION("Creating headers from request is successful")
    {
        res = HttpRequest::ParseHTTPHeaders(req);
        REQUIRE(!res.empty());
    }
    SECTION("Headers that are in the request are present")
    {
        res = HttpRequest::ParseHTTPHeaders(req);
        REQUIRE(res.find("Header") != res.end());
        REQUIRE(res.find("Cookie") != res.end());
    }
}

bool HeadersTest() {
    std::string tmp("GET / HTTP/1.1\r\nHeader:Value\r\nCookie:yo=aaaXz55==;data=rrr\r\n\r\n");
    std::vector<char> req(tmp.begin(), tmp.end());
    std::unordered_map<std::string, std::string> res = HttpRequest::ParseHTTPHeaders(req);
    if (res.find("Header") == res.end() ||
        res.find("Cookie") == res.end()) {
        FAIL
    }
    if (res["Header"] != "Value" || res["Cookie"] != "yo=aaaXz55==;data=rrr") {
        FAIL
    }
    std::cout << "Passed!" << std::endl;
    std::cout << "      No headers:";
    tmp = "GET / HTTP/1.1\r\n\r\n";
    req = std::vector<char>(tmp.begin(), tmp.end());
    try {
        res = HttpRequest::ParseHTTPHeaders(req);
        if (!res.empty()) {
            std::cout << "Failed!" << std::endl;
            return false;
        }
        std::cout << "Passed!" << std::endl;
    } catch (std::logic_error &err) {
        FAIL
    }
    return true;
}

bool QueryTest() {
    std::cout << "  Query tests:" << std::endl;
    std::cout << "      With data: ";
    std::string tmp("GET /thisisnice?my=data&your=tests HTTP/1.1\r\n\r\n");
    std::vector<char> v(tmp.begin(), tmp.end());
    GetRequest g(v);
    auto a = g.GetQuery();
    if (a.find("my") == a.end() || a.find("your") == a.end()) {
        FAIL
    }
    if (a["my"] != "data" || a["your"] != "tests") {
        FAIL
    }
    std::cout << "Passed!" << std::endl;
    std::cout << "      Without data: ";
    tmp = "GET / HTTP/1.1\r\n\r\n";
    v = std::vector<char>(tmp.begin(), tmp.end());
    g = GetRequest(v);
    a = g.GetQuery();
    if (!a.empty()) {
        FAIL
    }
    std::cout << "Passed!" << std::endl;
    return true;
}

bool UrlParamTest() {
    std::cout << "  Url-structure parameters: " << std::endl;
    std::cout << "      Parsing basic url template: ";
    std::unordered_map<int, std::string> spec;
    spec.insert(std::make_pair(1, "profile"));
    spec.insert(std::make_pair(2, "action"));
    std::string base = "users";
    Url urlTemplate(base, spec);
    std::string req("GET /users/admin/killall\r\n\r\n");
    std::vector<char> data(req.begin(), req.end());
    GetRequest r(data);
    r.PopulateParams(urlTemplate);
    if (!r.IsInUrlParams("profile") || !r.IsInUrlParams("action")) {
        FAIL
    }
    std::cout << "Passed!" << std::endl;
    return true;
}

bool NoDataTest() {
    std::cout << "  Build request with no data: ";
    std::vector<char> data;
    try {
        GetRequest g(data);
        FAIL
    } catch (std::runtime_error &err) {
        std::cerr << err.what() << std::endl;
        std::cout << "Passed!" << std::endl;
        return true;
    }
}

void TestGet() {
    bool isPassing = true;
    std::cout << "Testing GET request: " << std::endl;
    isPassing &= HeadersTest();
    isPassing &= QueryTest();
    isPassing &= UrlParamTest();
    isPassing &= NoDataTest();
    std::cout << "GET request test status: ";
    if (isPassing)
        std::cout << "\u001b[32m" << "\uF00C" << "\u001b[0m" << std::endl;
    else
        std::cout << "\u001b[31m" << "\uF00D" << "\u001b[0m" << std::endl;
}