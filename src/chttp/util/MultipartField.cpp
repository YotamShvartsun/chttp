#include "chttp/util/MultipartField.h"

#include <utility>
#include <algorithm>
#include <sstream>
#include <iterator>

/**
 * @brief This function parses multipart/form-data of a single file object
 * @param data raw multipart data of a single file, from boundary to boundary
 */
void MultipartField::FromMultipart(std::vector<char> data, const std::string &contentType) {
	std::string s(data.begin(), data.end());
    // Getting the boundary:
    std::string boundary = contentType.substr(contentType.find(';') + 1);
    boundary = boundary.substr(boundary.find("boundary=") + 9);
#ifdef DEBUG
    std::cout << "Boundary = " << boundary << std::endl;
#endif
    auto start = std::search(data.begin(), data.end(), boundary.begin(), boundary.end());
//    boundary = boundary + "--";
    auto end = std::search(start + boundary.size(), data.end(), boundary.begin(), boundary.end());
    std::vector<char> fileData(start, end);
    std::string newline("\r\n\r\n");
    auto headerEnd = std::search(fileData.begin(), fileData.end(), newline.begin(), newline.end());
    std::vector<char> headerVector(fileData.begin() + boundary.size(), headerEnd);
#ifdef DEBUG
    for (auto &a : headerVector)
        std::cout << a;
    std::cout << std::endl;
#endif
    std::string headerString(headerVector.begin(), headerVector.end());
    std::istringstream iss(headerString);
    std::vector<std::string> tmpHeaders;
    std::string token;
    while (std::getline(iss, token, '\n')) {
        tmpHeaders.push_back(token);
    }
#ifdef DEBUG
    for (auto &a : tmpHeaders)
        std::cout << "Header: " << a << std::endl;
#endif
    std::string type, hData;
    std::vector<std::string> contentDisposition;
    for (auto &header : tmpHeaders) {
    	if(header.size() == 1)
          continue;
        type = header.substr(0, header.find(':'));
        hData = header.substr(header.find(':'));
        if (type.find("Content-Disposition") != std::string::npos) {
            iss = std::istringstream(hData.substr(2));
            while (std::getline(iss, token, ';')) {
                contentDisposition.push_back(token);
            }
            for (auto &content : contentDisposition) {
            	if(content == "form-data")
                  continue;
                if(content.find("name") != std::string::npos && content.find("file") == std::string::npos)
                {
                    this->fieldName = content.substr(content.find('='));
                }
                else
                {
                    this->headers.insert(std::make_pair(content.substr(0, content.find('=')), content.substr(content.find('='))));
                }
            }
        }
        else
        {
            this->headers.insert((std::make_pair(header.substr(0, header.find(':')), header.substr(header.find(':') + 1))));
        }
    }
    this->data = std::vector<char>(headerEnd + newline.size(), fileData.end()-4);
#ifdef DEBUG
    for(auto & c : this->data)
        std::cout << c;
#endif
}

MultipartField::MultipartField(std::vector<char> data, const std::string &contentType) {
    this->FromMultipart(std::move(data), contentType);
}

const std::string &MultipartField::getFieldName() const {
    return fieldName;
}

const std::unordered_map<std::string, std::string> &MultipartField::getHeaders() const {
    return headers;
}

std::vector<char> MultipartField::getData() const {
    return this->data;
}
