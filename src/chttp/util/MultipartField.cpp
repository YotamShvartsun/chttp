/**
 * @file
 * @author Yotam Shvartsun <yotam.shvartsun@gmail.com>
 * @version 1.0
 * @section DESCRIPTION
 * This file contains the implementation for the MultipartField class, which
 * represents a multipart/form-data field in a POST request
 */
#include "chttp/util/MultipartField.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <utility>

/**
 * @brief This function parses multipart/form-data of a single file object
 * @param rawData raw multipart data of a single field, from boundary to
 * boundary
 */
void MultipartField::FromMultipart(std::vector<char> rawData,
                                   const std::string &contentType) {
  std::string s(rawData.begin(), rawData.end());
  // Get the boundary:
  std::string boundary = contentType.substr(contentType.find(';') + 1);
  boundary = boundary.substr(boundary.find("boundary=") + 9);
  auto start = std::search(rawData.begin(), rawData.end(), boundary.begin(),
                           boundary.end());
  auto end = std::search(start + boundary.size(), rawData.end(),
                         boundary.begin(), boundary.end());
  std::vector<char> fileData(start, end);
  std::string newline("\r\n\r\n");
  auto headerEnd = std::search(fileData.begin(), fileData.end(),
                               newline.begin(), newline.end());
  std::vector<char> headerVector(fileData.begin() + boundary.size(), headerEnd);
  std::string headerString(headerVector.begin(), headerVector.end());
  std::istringstream iss(headerString);
  std::vector<std::string> tmpHeaders;
  std::string token;
  while (std::getline(iss, token, '\n')) {
    tmpHeaders.push_back(token);
  }
  std::string type, hData;
  std::vector<std::string> contentDisposition;
  for (auto &header : tmpHeaders) {
    if (header.size() == 1)
      continue;
    type = header.substr(0, header.find(':'));
    hData = header.substr(header.find(':'));
    /**
     * The 'Content-Disposition' header stores the field's name, and some other
     * 'sub-headers', so it must be parsed
     */
    if (type.find("Content-Disposition") != std::string::npos) {
      iss = std::istringstream(hData.substr(2));
      while (std::getline(iss, token, ';')) {
        contentDisposition.push_back(token);
      }
      for (auto &content : contentDisposition) {
        if (content == "form-data")
          continue;
        std::string contentCopy(contentType);
        if (contentCopy.find("name") != std::string::npos &&
            contentCopy.find("file") == std::string::npos) {
          this->fieldName = contentCopy.substr(contentCopy.find('='));
        } else {
          this->headers.insert(
              std::make_pair(contentCopy.substr(0, contentCopy.find('=')),
                             contentCopy.substr(contentCopy.find('='))));
        }
      }
    } else {
      this->headers.insert(
          (std::make_pair(header.substr(0, header.find(':')),
                          header.substr(header.find(':') + 1))));
    }
  }
  // Set the body - everything except the headers
  this->data =
      std::vector<char>(headerEnd + newline.size(), fileData.end() - 4);
}

MultipartField::MultipartField(std::vector<char> data,
                               const std::string &contentType) {
  this->FromMultipart(std::move(data), contentType);
}

const std::string &MultipartField::getFieldName() const { return fieldName; }

const std::unordered_map<std::string, std::string> &
MultipartField::getHeaders() const {
  return headers;
}

std::vector<char> MultipartField::getData() const { return this->data; }
MultipartField::~MultipartField() { this->headers.clear(); }
