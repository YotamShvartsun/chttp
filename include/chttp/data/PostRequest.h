#pragma once

#include <memory>
#include <unordered_map>
#include <iostream>
#include <chttp/data/HttpRequest.h>

class PostRequest : HttpRequest
{
private:
    std::vector<char> body;
public:
    explicit PostRequest(std::vector<char> data);
    PostRequest & operator=(const PostRequest &);
    virtual void FromString(std::vector<char> data);
};