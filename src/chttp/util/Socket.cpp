#include "chttp/util/Socket.h"

Socket::Socket() {
#ifndef __linux__
    //winsock init here!
#endif
    this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
#ifdef __linux__
    if(this->sockfd < 0)
        throw std::runtime_error("Unable to create socket!");
#else
    if(this->sockfd == INVALID_SOCKET)
        throw std::runtime_error("Unable to create socket!");
#endif
}

Socket::~Socket() {
    close(this->sockfd);
}

Socket::Socket(SOCKET s) {
    this->sockfd = s;
}

void Socket::Bind(int port) {
    struct sockaddr_in serv_addr{};
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = port;
    if(bind(this->sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        throw std::runtime_error("Unable to bind");
    this->isBinded = true;
}

void Socket::Listen() {
    if(!this->isBinded)
        throw std::logic_error("Can't listen without bind!");
    /*
     * 128 is the default number in /proc/sys/net/core/somaxconn,
     * and according to `listen` man, a bigger number will change to the number in this file
     */
    if(listen(this->sockfd, 128) < 0)
        throw std::runtime_error("Unable to listen");
}

Socket Socket::Accept() {
    return Socket();
}

std::vector<char> Socket::GetData(uint size) {
    return std::vector<char>();
}

void Socket::SendData(std::vector<char> data) {

}
