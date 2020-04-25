/**
 * @file
 * @author Yotam Shvartsun <yotam.shvartsun@gmail.com>
 * @version 1.0
 * @section DESCRIPTION
 * This file is the main API: when users want to use CHttp, they can include
 * this instead of #include-ing everything
 */
#pragma once

#include <chttp/RequestHandler.h>
#include <chttp/Router.h>
#include <chttp/Server.h>
#include <chttp/data/GetRequest.h>
#include <chttp/data/HttpRequest.h>
#include <chttp/data/HttpResponse.h>
#include <chttp/data/PostRequest.h>
#include <chttp/util/MultipartField.h>
#include <chttp/util/Socket.h>
#include <chttp/util/Url.h>
#include <chttp/data/Cookie.hpp>