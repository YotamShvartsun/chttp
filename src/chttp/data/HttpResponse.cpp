/**
 * @file
 * @author Yotam Shvartsun <yotam.shvartsun@gmail.com>
 * @version 1.0
 * @section DESCRIPTION
 * This file contains the implementation of the HttpResponse class, and functions
 * to handle file reading
 */
#include <fstream>
#include <iostream>
#include <iterator>
// Since file op.s require using syscalls, the headers are different between
// OSs.
#ifndef _WIN32
#include <unistd.h>
#else
#include <Windows.h>
#include <io.h>
// define access to avoid changing the FileExists function
#define access _access_s
#endif

#include <chttp/data/HttpResponse.h>
#include <string>

/**
 * Check if file in path exists
 * @param path path to check
 * @return bool
 */
bool FileExists(const std::string &path) {
  return access(path.c_str(), 0) == 0;
}

std::string HttpResponse::BuildHeaders() {
  std::string
      raw; // it's much easier to do this as a number of string operations
  for (auto &pair : this->headers) {
    raw += pair.first + ": " + pair.second + "\r\n";
  }
  return raw;
}

HttpResponse::HttpResponse() : status(OK), dataSet(false) {}

void HttpResponse::SetStatus(HTTP_STATUS code) { this->status = code; }

void HttpResponse::Header(const std::string &key, const std::string &value) {
  this->headers.insert(std::make_pair(key, value));
}

void HttpResponse::SendFile(const std::string &path) {
  // If request body already set, throw error
  if (this->dataSet)
    throw std::logic_error("Body already set!");

  // If the file requested dose not exists, throw FileNotFound error
  if (!FileExists(path))
    throw std::runtime_error("File not found in path specified!");

  // read the file and set Content-Type
  std::string ext = path.substr(path.find_last_of('.'));
  std::ifstream file(path, std::ios::binary);
  this->payload = std::vector<char>((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());
  this->Header("Content-type", HttpResponse::MIME_TYPES.at(ext));
  this->dataSet = true;
}

void HttpResponse::Raw(const std::vector<char> &data) {
  if (this->dataSet)
    throw std::logic_error("Body already set!");
  this->payload = data;
  this->dataSet = true;
}

std::vector<char> HttpResponse::Format() {
  // Set the Content-Length header
  this->Header("Content-Length", std::to_string(this->payload.size()));
  // Build the status line
  std::string responseMeta = "HTTP/1.1 " + std::to_string(this->status) + " " +
                             HttpResponse::HttpCodeStrings.at(this->status) +
                             "\r\n";
  std::vector<char> response;

  responseMeta += this->BuildHeaders();
  responseMeta += "\r\n";
  // Add the body
  response = std::vector<char>(responseMeta.begin(), responseMeta.end());
  response.insert(response.end(), this->payload.begin(), this->payload.end());
  // HTTP response ends with \r\n\r\n
  response.push_back('\r');
  response.push_back('\n');
  response.push_back('\r');
  response.push_back('\n');
  return response;
}
