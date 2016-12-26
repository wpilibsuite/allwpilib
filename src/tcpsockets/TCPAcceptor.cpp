/*
   TCPAcceptor.cpp

   TCPAcceptor class definition. TCPAcceptor provides methods to passively
   establish TCP/IP connections with clients.

   ------------------------------------------

   Copyright © 2013 [Vic Hargrave - http://vichargrave.com]

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "tcpsockets/TCPAcceptor.h"

#include <cstdio>
#include <cstring>
#ifdef _WIN32
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#include "llvm/SmallString.h"
#include "support/Logger.h"
#include "tcpsockets/SocketError.h"

using namespace wpi;

TCPAcceptor::TCPAcceptor(int port, const char* address, Logger& logger)
    : m_lsd(0),
      m_port(port),
      m_address(address),
      m_listening(false),
      m_logger(logger) {
  m_shutdown = false;
#ifdef _WIN32
  WSAData wsaData;
  WORD wVersionRequested = MAKEWORD(2, 2);
  WSAStartup(wVersionRequested, &wsaData);
#endif
}

TCPAcceptor::~TCPAcceptor() {
  if (m_lsd > 0) {
    shutdown();
#ifdef _WIN32
    closesocket(m_lsd);
#else
    close(m_lsd);
#endif
  }
#ifdef _WIN32
  WSACleanup();
#endif
}

int TCPAcceptor::start() {
  if (m_listening) return 0;

  m_lsd = socket(PF_INET, SOCK_STREAM, 0);
  if (m_lsd < 0) {
    WPI_ERROR(m_logger, "could not create socket");
    return -1;
  }
  struct sockaddr_in address;

  std::memset(&address, 0, sizeof(address));
  address.sin_family = PF_INET;
  if (m_address.size() > 0) {
#ifdef _WIN32
    llvm::SmallString<128> addr_copy(m_address);
    addr_copy.push_back('\0');
    int res = InetPton(PF_INET, addr_copy.data(), &(address.sin_addr));
#else
    int res = inet_pton(PF_INET, m_address.c_str(), &(address.sin_addr));
#endif
    if (res != 1) {
      WPI_ERROR(m_logger, "could not resolve " << m_address << " address");
      return -1;
    }
  } else {
    address.sin_addr.s_addr = INADDR_ANY;
  }
  address.sin_port = htons(m_port);

  int optval = 1;
  setsockopt(m_lsd, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof optval);

  int result = bind(m_lsd, (struct sockaddr*)&address, sizeof(address));
  if (result != 0) {
    WPI_ERROR(m_logger, "bind() to port " << m_port
                                          << " failed: " << SocketStrerror());
    return result;
  }

  result = listen(m_lsd, 5);
  if (result != 0) {
    WPI_ERROR(m_logger, "listen() on port " << m_port
                                            << " failed: " << SocketStrerror());
    return result;
  }
  m_listening = true;
  return result;
}

void TCPAcceptor::shutdown() {
  m_shutdown = true;
#ifdef _WIN32
  ::shutdown(m_lsd, SD_BOTH);

  // this is ugly, but the easiest way to do this
  // force wakeup of accept() with a non-blocking connect to ourselves
  struct sockaddr_in address;

  std::memset(&address, 0, sizeof(address));
  address.sin_family = PF_INET;
  llvm::SmallString<128> addr_copy;
  if (m_address.size() > 0)
    addr_copy = m_address;
  else
    addr_copy = "127.0.0.1";
  addr_copy.push_back('\0');
  int size = sizeof(address);
  if (WSAStringToAddress(addr_copy.data(), PF_INET, nullptr,
                         (struct sockaddr*)&address, &size) != 0)
    return;
  address.sin_port = htons(m_port);

  fd_set sdset;
  struct timeval tv;
  int result = -1, valopt, sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd < 0) return;

  // Set socket to non-blocking
  u_long mode = 1;
  ioctlsocket(sd, FIONBIO, &mode);

  // Try to connect
  ::connect(sd, (struct sockaddr*)&address, sizeof(address));

  // Close
  ::closesocket(sd);

#else
  ::shutdown(m_lsd, SHUT_RDWR);
  int nullfd = ::open("/dev/null", O_RDONLY);
  if (nullfd >= 0) {
    ::dup2(nullfd, m_lsd);
    ::close(nullfd);
  }
#endif
}

std::unique_ptr<NetworkStream> TCPAcceptor::accept() {
  if (!m_listening || m_shutdown) return nullptr;

  struct sockaddr_in address;
#ifdef _WIN32
  int len = sizeof(address);
#else
  socklen_t len = sizeof(address);
#endif
  std::memset(&address, 0, sizeof(address));
  int sd = ::accept(m_lsd, (struct sockaddr*)&address, &len);
  if (sd < 0) {
    if (!m_shutdown)
      WPI_ERROR(m_logger, "accept() on port "
                              << m_port << " failed: " << SocketStrerror());
    return nullptr;
  }
  if (m_shutdown) {
#ifdef _WIN32
    closesocket(sd);
#else
    close(sd);
#endif
    return nullptr;
  }
  return std::unique_ptr<NetworkStream>(new TCPStream(sd, &address));
}
