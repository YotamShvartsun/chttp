#pragma  once

#include <iostream>
#include <vector>

class File {
private:
    std::string fieldName;
    std::string fileName;
    std::vector<char> data;
    void FromMultipart(std::vector<char> data);
public:
    File(std::vector<char>);
};