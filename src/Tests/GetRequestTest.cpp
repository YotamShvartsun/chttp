#include <iostream>
#include "chttp/util/Url.h"
#include "chttp/data/HttpRequest.h"
#include "chttp/data/GetRequest.h"
#include "thirdparty/catch.hpp"

#define FAIL return false;

TEST_CASE("GetRequest with headers", "[GetRequest][Headers]") {
    std::string tmp("GET / HTTP/1.1\r\nHeader:Value\r\nCookie:yo=aaaXz55==;data=rrr\r\n\r\n");
    std::vector<char> req(tmp.begin(), tmp.end());
    std::unordered_map<std::string, std::string> res;
    SECTION("Creating headers from request is successful") {
        res = HttpRequest::ParseHTTPHeaders(req);
        REQUIRE(!res.empty());
    }SECTION("Headers that are in the request are present") {
        res = HttpRequest::ParseHTTPHeaders(req);
        REQUIRE(res.find("Header") != res.end());
        REQUIRE(res.find("Cookie") != res.end());
    }SECTION("Headers should have the right values") {
        res = HttpRequest::ParseHTTPHeaders(req);
        REQUIRE(res.at("Header") == "Value");
        REQUIRE(res.at("Cookie") == "yo=aaaXz55==;data=rrr");
    }
}

TEST_CASE("GetRequest with no headers", "[GetRequest][Headers]") {
    std::string tmp = "GET / HTTP/1.1\r\n\r\n";
    std::vector<char> req = std::vector<char>(tmp.begin(), tmp.end());
    std::unordered_map<std::string, std::string> res;
    SECTION("Parsing with no headers dose not throw") {
        REQUIRE_NOTHROW(res = HttpRequest::ParseHTTPHeaders(req));
    }SECTION("Parsing with no headers dose not produce headers") {
        res = HttpRequest::ParseHTTPHeaders(req);
        REQUIRE(res.empty());
    }
}

TEST_CASE("Query string with values", "[GetRequest][Query]") {
    std::string tmp;
    std::vector<char> v;
    SECTION("Creating request is successful") {
        tmp = "GET /thisisnice?my=data&your=tests HTTP/1.1\r\n\r\n";
        v = std::vector<char>(tmp.begin(), tmp.end());
        REQUIRE_NOTHROW([&] {
            GetRequest req(v);
        });
    }
    SECTION("Request contains the query parts"){
        tmp = "GET /thisisnice?my=data&your=tests HTTP/1.1\r\n\r\n";
        v = std::vector<char>(tmp.begin(), tmp.end());
        GetRequest req(v);
        auto a = req.GetQuery();
        REQUIRE(a.find("my") != a.end());
        REQUIRE(a.find("your") != a.end());
        SECTION("Request query values are right"){
            REQUIRE(a["my"] == "data");
            REQUIRE(a["your"] == "tests");
        }
    }
}

TEST_CASE("Request with no query string","[GetRequest][Query]")
{
    std::string tmp = "GET / HTTP/1.1\r\n\r\n";
    std::vector<char> v(tmp.begin(), tmp.end());
    GetRequest req(v);
    auto a = req.GetQuery();
    REQUIRE(a.empty());
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

