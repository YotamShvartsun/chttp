/**
 * @file
 * @author Yotam Shvartsun <yotam.shvartsun@gmail.com>
 * @version 1.0
 * @section DESCRIPTION
 * This file contains the implementation of the PostRequest class
 */
#include <algorithm>
#include <chttp/data/PostRequest.h>
#include <chttp/util/MultipartField.h>
#include <vector>

void PostRequest::FromString(std::vector<char> data) {
  std::size_t firstSpace = 0;
  std::size_t lastSpace = 0;
  std::string asString(data.begin(), data.end());
  std::string firstLine = asString.substr(0, asString.find("\r\n"));
  // A post request must contain "POST" in the request line (aka first line)
  if (firstLine.find("POST") == std::string::npos) {
    throw std::runtime_error("Request is not a valid POST request!");
  }
  // Get the url
  firstSpace = firstLine.find_first_of(' ');
  lastSpace = firstLine.find_last_of(' ') - 1;
  if (firstSpace != std::string::npos) {
    this->rawUrl = firstLine.substr(firstSpace + 1, lastSpace - firstSpace);
  } else {
    throw std::runtime_error("Request is not a valid HTTP request!");
  }
  this->BuildQuery(rawUrl);
  // Parse the headers
  std::size_t headersEnd = asString.find("\r\n\r\n");
  std::string headers =
      asString.substr(firstLine.length(), headersEnd - firstLine.length());
  std::vector<char> tmp(headers.begin(), headers.end());
  this->headers = HttpRequest::ParseHTTPHeaders(tmp);
  try {
    // Parse the body
    long bodySize = std::stol(this->headers.at("Content-Length"));
    std::string bodyString = asString.substr(headersEnd + 4, bodySize);
    this->body = std::vector<char>(bodyString.begin(), bodyString.end());
  } catch (std::exception &e) {
#ifdef DEBUG
    std::cerr << "POST: " << e.what() << std::endl;
#endif
  }
}

PostRequest &PostRequest::operator=(const PostRequest &other) {
  // deep copy the body
  std::copy(other.rawUrl.begin(), other.rawUrl.end(), this->rawUrl.begin());
  this->query = other.query;
  this->headers = other.headers;
  this->urlParams = other.urlParams;
  return *this;
}

PostRequest::PostRequest(std::vector<char> data) { this->FromString(data); }

std::vector<char> PostRequest::GetBody() const { return this->body; }
