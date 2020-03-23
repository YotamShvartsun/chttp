/**
 * @file
 * @author Yotam Shvartsun <yotam.shvartsun@gmail.com>
 * @version 1.0
 * @section DESCRIPTION
 * This file contains the definition of the GetRequest class
*/
#pragma once

#include "chttp/data/HttpRequest.h"

/**
 * This class represents a GET request
 */
class GetRequest : public HttpRequest {
public:
  /**
   * Create a new request based on data received from a socket
   * @param data Data received from the socket
   */
  GetRequest(std::vector<char> data);
  /**
   * Copy constructor
   * @return Copy of input
   */
  GetRequest &operator=(const GetRequest &);
  /**
   * Create an object using the request data
   * @implements HttpRequest::FromString
   * @param data Request data
   */
  void FromString(std::vector<char> data) override;
};