/**
 * @file
 * @author Yotam Shvartsun <yotam.shvartsun@gmail.com>
 * @version 1.0
 * @section DESCRIPTION
 * This file contains the definition of the HttpResponse class.
 */
#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>

/**
 * A list of the different HTTP responses available.
 * @see <a href="https://en.wikipedia.org/wiki/List_of_HTTP_status_codes">A list
 * of HTTP response codes </a>
 */
enum HTTP_STATUS {
  Continue = 100,
  Switching_Protocols = 101,
  Processing = 102,
  Early_Hints = 103,
  OK = 200,
  Created = 201,
  Accepted = 202,
  Non_Authoritative_Information = 203,
  No_Content = 204,
  Reset_Content = 205,
  Partial_Content = 206,
  Multi_Status = 207,
  Already_Reported = 208,
  IM_Used = 226,
  Multiple_Choices = 300,
  Moved_Permanently = 301,
  Found = 302,
  See_Other = 303,
  Not_Modified = 304,
  Use_Proxy = 305,
  Switch_Proxy = 306,
  Temporary_Redirect = 307,
  Permanent_Redirect = 308,
  Bad_Request = 400,
  Unauthorized = 401,
  Payment_Required = 402,
  Forbidden = 403,
  Not_Found = 404,
  Method_Not_Allowed = 405,
  Not_Acceptable = 406,
  Proxy_Authentication_Required = 407,
  Request_Timeout = 408,
  Conflict = 409,
  Gone = 410,
  Length_Required = 411,
  Precondition_Failed = 412,
  Payload_Too_Large = 413,
  URI_Too_Long = 414,
  Unsupported_Media_Type = 415,
  Range_Not_Satisfiable = 416,
  Expectation_Failed = 417,
  Im_a_teapot = 418,
  Misdirected_Request = 421,
  Unprocessable_Entity = 422,
  Locked = 423,
  Failed_Dependency = 424,
  Too_Early = 425,
  Upgrade_Required = 426,
  Precondition_Required = 428,
  Too_Many_Requests = 429,
  Request_Header_Fields_Too_Large = 431,
  Unavailable_For_Legal_Reasons = 451,
  Internal_Server_Error = 500,
  Not_Implemented = 501,
  Bad_Gateway = 502,
  Service_Unavailable = 503,
  Gateway_Timeout = 504,
  HTTP_Version_Not_Supported = 505,
  Variant_Also_Negotiates = 506,
  Insufficient_Storage = 507,
  Loop_Detected = 508,
  Not_Extended = 510,
  Network_Authentication_Required = 511
};

/**
 * This class represents HTTP response object.
 * It also serializes the response to a valid response string, based on the
 * payload
 * @note Any response object can change it's payload once
 */
class HttpResponse {
 private:
  /**
   * The Reason phrase for each status code
   * @see <a href="https://tools.ietf.org/html/rfc2616#section-6.1">RFC 2616,
   * section 6.1 - HTTP response status line</a>
   */
  const std::unordered_map<HTTP_STATUS, std::string> HttpCodeStrings = {
      {Continue, "Continue"},
      {Switching_Protocols, "Switching Protocols"},
      {Processing, "Processing"},
      {Early_Hints, "Early Hints"},
      {OK, "OK"},
      {Created, "Created"},
      {Accepted, "Accepted"},
      {Non_Authoritative_Information, "Non-Authoritative Information"},
      {No_Content, "No Content"},
      {Reset_Content, "Reset Content"},
      {Partial_Content, "Partial Content"},
      {Multi_Status, "Multi Status"},
      {Already_Reported, "Already Reported"},
      {IM_Used, "IM Used"},
      {Multiple_Choices, "Multiple Choices"},
      {Moved_Permanently, "Moved Permanently"},
      {Found, "Found"},
      {See_Other, "See Other"},
      {Not_Modified, "Not Modified"},
      {Use_Proxy, "Use Proxy"},
      {Switch_Proxy, "Switch Proxy"},
      {Temporary_Redirect, "Temporary Redirect"},
      {Permanent_Redirect, "Permanent Redirect"},
      {Bad_Request, "Bad Request"},
      {Unauthorized, "Unauthorized"},
      {Payment_Required, "Payment Required"},
      {Forbidden, "Forbidden"},
      {Not_Found, "Not Found"},
      {Method_Not_Allowed, "Method Not Allowed"},
      {Not_Acceptable, "Not Acceptable"},
      {Proxy_Authentication_Required, "Proxy Authentication Required"},
      {Request_Timeout, "Request Timeout"},
      {Conflict, "Conflict"},
      {Gone, "Gone"},
      {Length_Required, "Length Required"},
      {Precondition_Failed, "Precondition Failed"},
      {Payload_Too_Large, "Payload Too Large"},
      {URI_Too_Long, "URI Too Long"},
      {Unsupported_Media_Type, "Unsupported Media Type"},
      {Range_Not_Satisfiable, "Range Not Satisfiable"},
      {Expectation_Failed, "Expectation Failed"},
      {Im_a_teapot, "I'm a teapot"},
      {Misdirected_Request, "Misdirected Request"},
      {Unprocessable_Entity, "Unprocessable Entity"},
      {Locked, "Locked"},
      {Failed_Dependency, "Failed Dependency"},
      {Too_Early, "Too Early"},
      {Upgrade_Required, "Upgrade Required"},
      {Precondition_Required, "Precondition Required"},
      {Too_Many_Requests, "Too Many Requests"},
      {Request_Header_Fields_Too_Large, "Request Header Fields Too Large"},
      {Unavailable_For_Legal_Reasons, "Unavailable For Legal Reasons"},
      {Internal_Server_Error, "Internal Server Error"},
      {Not_Implemented, "Not Implemented"},
      {Bad_Gateway, "Bad Gateway"},
      {Service_Unavailable, "Service Unavailable"},
      {Gateway_Timeout, "Gateway Timeout"},
      {HTTP_Version_Not_Supported, "HTTP Version Not Supported"},
      {Variant_Also_Negotiates, "Variant Also Negotiates"},
      {Insufficient_Storage, "Insufficient Storage"},
      {Loop_Detected, "Loop Detected"},
      {Not_Extended, "Not_Extended"},
      {Network_Authentication_Required, "Network Authentication Required"}};
  /**
   * A very basic list of MIME-types associated with file extensions
   */
  const std::unordered_map<std::string, std::string> MIME_TYPES = {
      {".js", "application/javascript"},
      {".jsonp", "application/javascript"},
      {".json", "application/json"},
      {".gif", "image/gif"},
      {".jpeg", "image/jpeg"},
      {".jpg", "image/jpeg"},
      {".mp3", "audio/mpeg"},
      {".gz", "application/gzip"},
      {".png", "image/png"},
      {".html", "text/html"},
      {".css", "text/css"}};

  /**
   * Status of the request
   * Default value is 200 OK
   */
  HTTP_STATUS status = OK;
  /**
   * Payload of the response - response body
   */
  std::vector<char> payload;
  /**
   * Response headers
   */
  std::unordered_map<std::string, std::string> headers;
  /**
   * New cookies added by the response
   */
  std::unordered_map<std::string, std::string> newCookies;
  /**
   * Holds whether the response body has already been given value
   */
  bool dataSet;

  /**
   * Build the header part of the response, based on ``HttpResponse::headers``
   * @return A valid response header field
   */
  std::string BuildHeaders();

  public:
  /**
   * Build an empty HttpResponse object
   */
  HttpResponse();

  /**
   * @brief This function sets the response status
   * @param code the status code
   */
  void SetStatus(HTTP_STATUS);

  /**
   * @brief This function adds another header and a value to the response header
   * list
   * @note if a header has already been set, this function will replace its
   * value
   * @param key name of the header
   * @param value value for header key
   */
  void Header(const std::string &, const std::string &);

  /**
   * @brief send the content of a given file
   * @param path the path of the file
   * @throws std::logic_error if this function or Raw has been called already
   */
  void SendFile(const std::string &);

  /**
   * @brief send the data in buffer
   * @param data data to send
   * @throws std::logic_error if this function or SendFile has been called
   */
  void Raw(const std::vector<char> &);
  void Raw(std::string);
  /**
   * @brief format this response to text
   * @return this response as a valid HTTP response
   */
  std::vector<char> Format();
  /**
   * @brief Checks if the payload of this request has been set
   * @return true if set, false if not
   */
  bool isSet() const;
  /**
   * Redirect the request to a url
   * @param url the new url
   */
  void Redirect(std::string url);

  void RemoveCookie(const std::string &cookieName);

  void AddCookie(std::string cookieName, std::string cookieValue);
  void AddCookie(std::string cookieName, std::string cookieValue, int maxAge);
};