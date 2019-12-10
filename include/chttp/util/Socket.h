#pragma once

#include <iostream>
#include <vector>

#ifdef __linux__

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#define SOCKET int
#else
#include <Winsock2.h>
#endif

class Socket {
private:
    bool isBinded = false;
    SOCKET sockfd;
    explicit Socket(SOCKET s);
public:
    explicit Socket();
    ~Socket();
    void Bind(int port);
    void Listen();
    Socket Accept();
    std::vector<char> GetData(uint size);
    void SendData(std::vector<char> data);
};