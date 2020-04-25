#pragma once

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

class Cookie {
private:
	/**
	 * Name of the cookie
	 */
	std::string CookieName;
	/**
	 * Value stored in the cookie
	 */
	std::string Value;

public:
	Cookie() = default;
	Cookie(std::string headerPart);
	/**
	 * @brief Get the name of this cookie
	 * @return std::string name of the cookie
	 */
	std::string GetName() const;
	/**
	 * @brief Get the value stored in this cookie
	 * @return Value stored in cookie
	 */
	std::string GetValue() const;
};

Cookie::Cookie(std::string headerPart) {
	std::stringstream asStream(headerPart);
	std::string token;
	short iterNumber = 1;
	while (std::getline(asStream, token, '=')) {
		switch (iterNumber) {
			case 1: {
				this->CookieName = token;
				break;
			}
			case 2: {
				this->Value = token;
				break;
			}
			default: {
				throw std::invalid_argument("Invalid cookie!");
			}
		}
        iterNumber++;
	}
}

std::string Cookie::GetName() const {
	return this->CookieName;
}
std::string Cookie::GetValue() const {
	return this->Value;
}

class CookieJar{
private:
	std::unordered_map<std::string, Cookie> cookies;

public:
	CookieJar(const std::string& CookieHeader);
	bool IsInJar(const std::string& cookieName);
	std::string GetCookieValue(std::string cookieName);
	std::vector<std::string> GetCookieNames();
};

CookieJar::CookieJar(const std::string& CookieHeader) {
	Cookie tmp;
    std::string cookiePart;
	std::stringstream header(CookieHeader);

	while(std::getline(header, cookiePart, ';')){
		if(cookiePart[0] == ' '){
			cookiePart = cookiePart.substr(1);
		}
		tmp = Cookie(cookiePart);
		this->cookies.insert(std::make_pair(tmp.GetName(), tmp));
	}
}

bool CookieJar::IsInJar(const std::string& cookieName) {
	return this->cookies.find(cookieName) != this->cookies.end();
}

std::string CookieJar::GetCookieValue(std::string cookieName) {
	if(!this->IsInJar(cookieName))
		throw std::invalid_argument("Cookie not found");
	return this->cookies.at(cookieName).GetValue();
}
std::vector<std::string> CookieJar::GetCookieNames() {
	std::vector<std::string> cookieNames;
	for(auto &pair : this->cookies)
		cookieNames.push_back(pair.first);
	return cookieNames;
}
