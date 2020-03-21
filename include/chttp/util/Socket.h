/**
 * @file
 * @author Yotam Shvartsun <yotam.shvartsun@gmail.com>
 * @version 1.0
 * @section DESCRIPTION
 * This file contains the definition of Socket - a TCP socket wrapper class,
 * used by CHttp project
 * @note Since this is a part of a server framework, this class will not support
 * any type of client side operations (`connect`, .etc).
 */
#pragma once

#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <system_error>
#include <vector>

#include <atomic>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SOCKET int

/**
 * This class is a RAII wrapper for a TCP socket
 */
class Socket {
private:
  /**
   * Number of references to this object. When it points to 0, the socket will
   * be closed Whenever an object is copied, the value will increase.
   */
  std::atomic<int *> referenceCount;
  /**
   * hold wheather the Socket object is bound or not.
   * Changes to true when calling the Socket::Bind function and to false on
   * Socket::Close
   */
  bool isBound = false;
  /**
   * For a listening socket - the address.
   */
  struct sockaddr_in server_addr {};
  /**
   * The file descriptor of the socket
   */
  SOCKET sockfd;

  /**
   * constructor - Create a Socket instance based on a given file descriptor
   * @param fileDes a file descriptor
   * @note This function dose not check if the file descriptor is valid
   */
  explicit Socket(SOCKET fileDes);

public:
  /**
   * constructor - Create a new Socket object
   * @throws std::system_error when socket creation fails.
   */
  Socket();

  /**
   * constructor - Copy an existing Socket object to a new one
   * @param other
   */
  Socket(const Socket &other);

  /**
   * destructor - Destroys a Socket object
   * Closes the socket if its working
   */
  ~Socket();

  /**
   * Closes the socket object
   * Makes this->sockfd -1 - INVALID_SOCKET
   * @throws std::system_error if failed to close the socket
   */
  void Close();

  /**
   * Bind this socket to a given port
   * @param port
   * @throws runtime_error if the socket has bound a port already, and now
   * wishing to bind a different one
   * @throws system_error when syscalls (setsockopt & bind) fails.
   */
  void Bind(int port);

  /**
   * Change the socket state to LISTEN state if possible
   * @throws logic_error when `this` is an unbound socket
   * @throws system_error if `listen` syscall fails
   */
  void Listen();

  /**
   * Accepts new connection
   * @note This is a blocking function - it will wait for clients.
   * @throws logic_error if `this` is an unbound socket
   * @throws system_error if accept syscall failed
   * @return A new Socket object, representing the client
   */
  Socket Accept();

  /**
   * Receive bytes from this socket
   * @note This function will not wait until there are `maxBufferSize` bytes
   * sent
   * @param maxBufferSize Maximum buffer size requested
   * @return Data received from socket
   * @throws runtime_error If the socket is closed
   * @throws system_error If the ::recv syscall fails
   */
  std::vector<char> GetData(uint maxBufferSize);

  /**
   * Send data using the socket object
   * @param data Data to send
   * @throws system_error If there was an error during the 'send' syscall
   * @throws runtime_error If the socket is closed
   */
  void SendData(std::vector<char> data);
};