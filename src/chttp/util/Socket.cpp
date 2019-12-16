#include "chttp/util/Socket.h"

Socket::Socket() {
#ifndef __linux__
    //winsock init here!
#endif
    this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
#ifdef __linux__
    if (this->sockfd < 0)
        throw std::runtime_error("Unable to create socket!");
#else
    if(this->sockfd == INVALID_SOCKET)
        throw std::runtime_error("Unable to create socket!");
#endif
}

Socket::~Socket() {
    if (this->sockfd > 0) {
        close(this->sockfd);
    }
}

Socket::Socket(SOCKET s) {
    this->sockfd = s;
}

void Socket::Bind(int port) {
    int opt = 1;
    if (setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    this->sLen = sizeof(this->serv_addr);
    bzero((char *) &(this->serv_addr), sizeof(this->serv_addr));
    this->serv_addr.sin_family = AF_INET;
    this->serv_addr.sin_addr.s_addr = INADDR_ANY;
    this->serv_addr.sin_port = htons(port);
    if (bind(this->sockfd, (struct sockaddr *) &(this->serv_addr), sizeof(this->serv_addr)) < 0)
        throw std::runtime_error("Unable to bind");
    this->isBinded = true;
}

void Socket::Listen() {
    if (!this->isBinded)
        throw std::logic_error("Can't listen without bind!");
    /*
     * 128 is the default number in /proc/sys/net/core/somaxconn,
     * and according to `listen` man, a bigger number will change to the number in this file
     */
    if (listen(this->sockfd, SOMAXCONN) < 0)
        throw std::runtime_error("Unable to listen");
}

Socket Socket::Accept() {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(struct sockaddr_in);
    SOCKET sock = ::accept(this->sockfd, reinterpret_cast<sockaddr *>(&client_addr), &client_addr_len);
    return Socket(sock);
}

std::vector<char> Socket::GetData(uint size) {
    std::vector<char> res;
    char tmp;
    int recvCode;
    if (this->sockfd < 0) {
#ifdef DEBUG
        std::cerr << "Trying to read from closed socket!" << std::endl;
#endif
        return res;
    }
    for (int i = 0; i < size; i++) {
#ifdef  __linux__
        if ((recvCode = recv(this->sockfd, &tmp, 1, 0)) < 0) {
#ifdef DEBUG
            std::cerr << "Error on GetData. Sockfd:" << this->sockfd << " Socket object in " << std::hex << this;
            std::cerr << strerror(errno) << std::endl;
#endif
        } else if (recvCode == 0) {
            ::close(this->sockfd);
            this->sockfd = -1;
            return res; // socket closed!
        } else {
            res.push_back(tmp);
        }
#endif
    }
}

void Socket::SendData(std::vector<char> data) {

    char *toSend = new char[data.size() + 1];
    std::copy(data.begin(), data.end(), toSend);
    if (this->sockfd < 0) {
#ifdef DEBUG
        std::cerr << "Trying to read from closed socket!" << std::endl;
#endif
        return;
    }
#ifdef __linux__
    if (send(this->sockfd, toSend, data.size() + 1, 0) <= 0) {
#ifdef DEBUG
        std::cerr << "Error on GetData. Sockfd:" << this->sockfd << " Socket object in " << std::hex << this;
        std::cerr << strerror(errno) << std::endl;
        delete[] toSend;
        ::close(this->sockfd);
        throw std::runtime_error("Socket error!");
#endif
    }
#endif
    delete[] toSend;
}
