#pragma once

#include <iostream>
#include <vector>

#ifdef __linux__

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#define SOCKET int
#else
#include <Winsock2.h>
typedef int socklen_t;
#pragma comment(lib, "Ws2_32.lib")
#endif

class Socket {
private:
    bool isBinded = false;
#ifdef __linux__
    socklen_t sLen;
#else
	static int sockCreated;
#endif
    struct sockaddr_in serv_addr;
    SOCKET sockfd;
    explicit Socket(SOCKET s);
public:
    Socket();
    ~Socket();
    void Bind(int port);
    void Listen();
    Socket Accept();
    std::vector<char> GetData(unsigned int size);
    void SendData(std::vector<char> data);
};