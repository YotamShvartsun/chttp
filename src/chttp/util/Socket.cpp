/**
 * @file
 * @author Yotam Shvartsun <yotam.shvartsun@gmail.com>
 * @version 1.0
 * @section DESCRIPTION
 * This file contains the implementation of the Socket class - a part of the
 * chttp framework. The Socket class is a RAII wrapper class for a TCP socket -
 * when the destructor of this class is called, the socket it represents is
 * closed.
 * @note Since this is a part of a server framework, this class will not support
 * any type of client side operations (`connect`, .etc).
 */
#define DEBUG
#include <chttp/util/Socket.h>

#include <string>
#ifdef DEBUG
#ifndef _WIN32
#include <arpa/inet.h> // in order to print the IP address of a client
#endif
#endif

#ifdef _WIN32
bool Socket::ShouldCallWSAStartup = true;
#endif

// constructor
Socket::Socket() : referenceCount() {
#ifdef _WIN32
  if (Socket::ShouldCallWSAStartup) {
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) != NO_ERROR) {
      wprintf(L"Error at WSAStartup()\n");
      exit(-1);
    }
    Socket::ShouldCallWSAStartup = false;
  }
#endif // _WIN32

  this->referenceCount.store(new int(1));
  this->sockfd =
      ::socket(AF_INET, SOCK_STREAM, 0); // create the file descriptor
  if (this->sockfd < 0)
    throw std::system_error(errno, std::system_category(),
                            "Unable to create socket!");
}

// constructor
Socket::Socket(const Socket &other) : referenceCount() {
  this->referenceCount.store(other.referenceCount.load());
  (*this->referenceCount.load())++;
  this->sockfd = other.sockfd;
}

// constructor
Socket::Socket(SOCKET fileDes) : referenceCount() {
  this->referenceCount.store(new int(1));
  this->sockfd = fileDes;
}

// destructor
Socket::~Socket() {
  (*this->referenceCount.load())--;
  if ((*this->referenceCount.load()) == 0) {
    this->Close();
    delete this->referenceCount.load();
    this->referenceCount.store(nullptr);
  }
}

void Socket::Bind(int port) {
  if (this->isBound) {
    // If trying to bind a different port, throw an error - a socket can only
    // bind one port If trying to bind the same port again, do nothing.
    if (this->server_addr.sin_port !=
        htons(port)) { // parse network-byte order to host-byte order (regular
                       // numbers)
      throw std::runtime_error(
          "Socket already listening on port " +
          std::to_string(::ntohs(this->server_addr.sin_port)));
    }
    return;
  }
  int optionValue = 1;
  // The SO_REUSEADDR | SO_REUSEPORT allows the socket to bind an address in
  // time-wait state. Allows a TCP socket to use an address which was used by
  // another (killed) process.
#ifndef _WIN32
  if (setsockopt(this->sockfd, SOL_SOCKET,
                 SO_REUSEADDR | SO_REUSEPORT, // NOLINT
                 &optionValue, sizeof(optionValue)) != 0) {
    throw std::system_error(errno, std::system_category(),
                            "Unable to setup socket!");
  }
  // make sure that this->server_addr is clean - all data in it is ours.
  bzero((char *)&(this->server_addr), sizeof(this->server_addr));
  this->server_addr.sin_family = AF_INET;
  this->server_addr.sin_addr.s_addr = INADDR_ANY;
  this->server_addr.sin_port =
      htons(port); // parse host-byte order to network-byte order

  if (bind(this->sockfd, (struct sockaddr *)&(this->server_addr),
           sizeof(this->server_addr)) < 0)
    throw std::system_error(errno, std::system_category(), "Unable to bind");
#else
  if (setsockopt(this->sockfd, SOL_SOCKET,
                 SO_REUSEADDR, // NOLINT
                 (char *)&optionValue, sizeof(optionValue)) == SOCKET_ERROR) {
    throw std::system_error(errno, std::system_category(),
                            "Unable to setup socket!");
  }
  this->server_addr = {0};
  this->server_addr.sin_port = htons(port);
  this->server_addr.sin_family = AF_INET;
  this->server_addr.sin_addr.s_addr = INADDR_ANY;
  if (::bind(this->sockfd, (struct sockaddr *)&(this->server_addr),
             sizeof(this->server_addr)) == SOCKET_ERROR)
    throw std::runtime_error("Unable to bind");
#endif // !_WIN32
  this->isBound = true;
}

void Socket::Listen() {
  // When 'bind' is not called and listen is called, OS will bind a random port
  // for the software. Since Socket::Listen method is used in order to build a
  // server (with a well-defined port), this scenario should raise an error
  if (!this->isBound)
    throw std::logic_error("Can't listen on unbound socket");

  // 128 is the default number in /proc/sys/net/core/somaxconn,
  // and according to `listen` man, a bigger number will change to the number in
  // this file. The constant SOMAXCONN is the value of
  // /proc/sys/net/core/somaxconn
  if (::listen(this->sockfd, SOMAXCONN) < 0)
    throw std::system_error(errno, std::system_category(), "Unable to listen");
}

Socket Socket::Accept() {
  if (!this->isBound) { // If the socket is unbound, it cannot accept clients.
    throw std::logic_error("Unable to accept from a non-bound socket!");
  }

  struct sockaddr_in client_addr {};
  socklen_t client_addr_len = sizeof(struct sockaddr_in);
  SOCKET sock =
      ::accept(this->sockfd, reinterpret_cast<sockaddr *>(&client_addr),
               &client_addr_len);
#ifdef DEBUG
  std::cout << "Client connected!" << std::endl
            << "IP: " << ::inet_ntoa(client_addr.sin_addr)
            << " PORT: " << client_addr.sin_port << std::endl;
#endif
  if (sock < 0) { // client socket is invalid
    throw std::system_error(errno, std::system_category(),
                            "Connection failed!");
  }
  struct timeval timeout {
    1, 0
  };
  if (::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                   sizeof(timeout)) < 0) {
    throw std::runtime_error("Cannot change socket settings");
  }
  return Socket(sock);
}

// maxBufferSize define the max reading buffer maxBufferSize
std::vector<char> Socket::GetData(unsigned int maxBufferSize) {
  std::vector<char> res;
  if (maxBufferSize == 0) // if requesting to read 0 bytes, return empty array
    return res;
  std::unique_ptr<char[]> rawData = std::make_unique<char[]>(maxBufferSize);
  int recvCode;
  if (this->sockfd < 0) {
    throw std::runtime_error("Unable to read data from a closed socket!");
  }
  // If 'maxBufferSize' is bigger then the actual sent data length ::recv will
  // output all data in the socket buffer If the socket buffer is empty, ::recv
  // will wait
  recvCode = ::recv(this->sockfd, rawData.get(), maxBufferSize, 0);
  if (recvCode > 0) {
    std::copy(rawData.get(), rawData.get() + recvCode, std::back_inserter(res));
  } else if (recvCode == 0) {
    // When recv returns 0, it means that the client disconnected.
    this->Close();
    throw std::runtime_error("Unable to read data from a closed socket!");
  } else {
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
      // when recv returns -1, it means that something is wrong - throw an
      // error.
      throw std::system_error(errno, std::system_category());
    } else {
      throw std::runtime_error("Timed out");
    }
  }
  return res;
}

void Socket::SendData(std::vector<char> data) {
  if (this->sockfd < 0) {
    throw std::runtime_error("Unable to send data to a closed socket!");
  }
  std::unique_ptr<char[]> toSend = std::make_unique<char[]>(data.size() + 1);
  std::copy(data.begin(), data.end(), toSend.get()); // copy our data to a char*
  if (::send(this->sockfd, toSend.get(), data.size(), 0) < 0) {
    this->Close();
    throw std::system_error(errno, std::system_category());
  }
}

void Socket::Close() {
  this->isBound = false;
#ifndef _WIN32
  if (this->sockfd > 0) {
    if (::close(this->sockfd)) {
      // handle errors
      this->sockfd = -1;
      throw std::system_error(errno, std::system_category(),
                              "Unable to close socket");
    }
    this->sockfd = -1;
  }
#else
  if (this->sockfd != INVALID_SOCKET) {
    if (::closesocket(this->sockfd)) {
      this->sockfd = INVALID_SOCKET;
      throw std::system_error(errno, std::system_category(),
                              "Unable to close socket");
    }
  }
  this->sockfd = INVALID_SOCKET;
#endif // !_WIN32
}
bool Socket::IsClosed() const {
#ifdef _WIN32
  return this->sockfd == INVALID_SOCKET;
#else
  return this->sockfd == -1;
#endif
}
