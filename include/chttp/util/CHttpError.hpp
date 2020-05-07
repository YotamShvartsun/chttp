#pragma once

#include <stdexcept>
#include <chttp/data/HttpResponse.h>

class CHttpError : public std::exception {
private:
	std::string errorMessage;
    HTTP_STATUS HTTPCode = Internal_Server_Error;
public:
	explicit CHttpError(const char *msg) : errorMessge(msg) {}
	explicit CHttpError(std::string &message) : errorMessage(message) {}
    explicit CHttpError(const char *msg, HTTP_STATUS errorCode) : errorMessge(msg), HTTPCode(errorCode) {}
    explicit CHttpError(std::string &message, HTTP_STATUS errorCode) : errorMessage(message), HTTPCode(errorCode) {}
	virtual ~CHttpError() throw() {}
	virtual const char * what() const throw() {
		return this->errorMessage.c_str();
	}
	std::string what() const throw() {
		return this->errorMessage;
	}
	HTTP_STATUS GetErrorCode() const{
		return this->HTTPCode;
	}
};