#pragma  once

#include <iostream>
#include <vector>
#include <unordered_map>

class MultipartField {
private:
    std::string fieldName;
    std::string fileName;
    std::vector<char> data;
    std::unordered_map<std::string, std::string> headers;
    void FromMultipart(std::vector<char> data, const std::string& contentType);
public:
    MultipartField(std::vector<char> data, const std::string& contentType);
    const std::string &getFieldName() const;
    const std::unordered_map<std::string, std::string> &getHeaders() const;
};