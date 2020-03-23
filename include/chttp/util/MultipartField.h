/**
 * @file
 * @author Yotam Shvartsun <yotam.shvartsun@gmail.com>
 * @version 1.0
 * @section DESCRIPTION
 * This file contains the definitions for the MultipartField class, which
 * represents a multipart/form-data field in a POST request
 */
#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>

/**
 * This class represents a multipart/form-data object
 */
class MultipartField {
private:
  /**
   * Name of the field
   */
  std::string fieldName;
  /**
   * Body of the field
   */
  std::vector<char> data;
  /**
   * A map containing the headers - name:value
   */
  std::unordered_map<std::string, std::string> headers;
  /**
   * Set this class instance based on the data received from the client
   * @param rawData The POST request body
   * @param contentType The 'Content-Type' header of the request. Used to
   * determine the boundary of the form
   */
  void FromMultipart(std::vector<char> rawData, const std::string &contentType);

public:
  /**
   * Create a new instance based on the data received from the client
   * @param data The POST request body
   * @param contentType The 'Content-Type' header of the request. Used to
   * determine the boundary of the form
   */
  MultipartField(std::vector<char> data, const std::string &contentType);
  /**
   * Get this field name
   * @return This field name
   */
  const std::string &getFieldName() const;
  /**
   * Get a map of all headers
   * @return Map of headers
   */
  const std::unordered_map<std::string, std::string> &getHeaders() const;
  /**
   * Get the data stored in this field's body
   * @return This field's body
   */
  std::vector<char> getData() const;

  ~MultipartField();
};