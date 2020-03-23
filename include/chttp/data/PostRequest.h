/**
 * @file
 * @author Yotam Shvartsun <yotam.shvartsun@gmail.com>
 * @version 1.0
 * @section DESCRIPTION
 * This file contains the definition of the PostRequest class
 */
#pragma once

#include <chttp/data/HttpRequest.h>
#include <iostream>
#include <memory>
#include <unordered_map>

/**
 * This class represents POST request, with a well defined body.
 * @note For developers: This class is an example of a data-holding request.
 * When implementing PUT, use this code as an example
 */
class PostRequest : public HttpRequest {

private:
  /**
   * Body of the request
   */
  std::vector<char> body;

public:
  /**
   * Create a new instance of PostRequest based on the data received from the
   * client
   * @param data Data received from the client
   */
  explicit PostRequest(std::vector<char> data);
  /**
   * Copy an instance of PostRequest
   * @return A copy of the instance
   */
  PostRequest &operator=(const PostRequest &);

  void FromString(std::vector<char> data) override;
  /**
   * Get the body of this request
   * @return Body of the request
   */
  std::vector<char> GetBody() const;
};