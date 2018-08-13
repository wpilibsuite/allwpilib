/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/UDPClient.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#include "wpi/Logger.h"
#include "wpi/SocketError.h"

using namespace wpi;

UDPClient::UDPClient(Logger& logger) : UDPClient("", logger) {}

UDPClient::UDPClient(const Twine& address, Logger& logger)
    : m_lsd(0), m_port(0), m_address(address.str()), m_logger(logger) {}

UDPClient::UDPClient(UDPClient&& other)
    : m_lsd(other.m_lsd),
      m_port(other.m_port),
      m_address(std::move(other.m_address)),
      m_logger(other.m_logger) {
  other.m_lsd = 0;
  other.m_port = 0;
}

UDPClient::~UDPClient() {
  if (m_lsd > 0) {
    shutdown();
  }
}

UDPClient& UDPClient::operator=(UDPClient&& other) {
  if (this == &other) return *this;
  shutdown();
  m_logger = other.m_logger;
  m_lsd = other.m_lsd;
  m_address = std::move(other.m_address);
  m_port = other.m_port;
  other.m_lsd = 0;
  other.m_port = 0;
  return *this;
}

int UDPClient::start() { return start(0); }

int UDPClient::start(int port) {
  if (m_lsd > 0) return 0;

#ifdef _WIN32
  WSAData wsaData;
  WORD wVersionRequested = MAKEWORD(2, 2);
  WSAStartup(wVersionRequested, &wsaData);
#endif

  m_lsd = socket(AF_INET, SOCK_DGRAM, 0);

  if (m_lsd < 0) {
    WPI_ERROR(m_logger, "could not create socket");
    return -1;
  }

  struct sockaddr_in addr;
  std::memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  if (m_address.size() > 0) {
#ifdef _WIN32
    SmallString<128> addr_copy(m_address);
    addr_copy.push_back('\0');
    int res = InetPton(PF_INET, addr_copy.data(), &(addr.sin_addr));
#else
    int res = inet_pton(PF_INET, m_address.c_str(), &(addr.sin_addr));
#endif
    if (res != 1) {
      WPI_ERROR(m_logger, "could not resolve " << m_address << " address");
      return -1;
    }
  } else {
    addr.sin_addr.s_addr = INADDR_ANY;
  }
  addr.sin_port = htons(port);

  if (port != 0) {
#ifdef _WIN32
    int optval = 1;
    setsockopt(m_lsd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
               reinterpret_cast<char*>(&optval), sizeof optval);
#else
    int optval = 1;
    setsockopt(m_lsd, SOL_SOCKET, SO_REUSEADDR,
               reinterpret_cast<char*>(&optval), sizeof optval);
#endif
  }

  int result = bind(m_lsd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
  if (result != 0) {
    WPI_ERROR(m_logger, "bind() failed: " << SocketStrerror());
    return result;
  }
  m_port = port;
  return 0;
}

void UDPClient::shutdown() {
  if (m_lsd > 0) {
#ifdef _WIN32
    ::shutdown(m_lsd, SD_BOTH);
    closesocket(m_lsd);
    WSACleanup();
#else
    ::shutdown(m_lsd, SHUT_RDWR);
    close(m_lsd);
#endif
    m_lsd = 0;
    m_port = 0;
  }
}

int UDPClient::send(ArrayRef<uint8_t> data, const Twine& server, int port) {
  // server must be a resolvable IP address
  struct sockaddr_in addr;
  std::memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  SmallVector<char, 128> addr_store;
  StringRef remoteAddr = server.toNullTerminatedStringRef(addr_store);
  if (remoteAddr.empty()) {
    WPI_ERROR(m_logger, "server must be passed");
    return -1;
  }

#ifdef _WIN32
  int res = InetPton(AF_INET, remoteAddr.data(), &(addr.sin_addr));
#else
  int res = inet_pton(AF_INET, remoteAddr.data(), &(addr.sin_addr));
#endif
  if (res != 1) {
    WPI_ERROR(m_logger, "could not resolve " << server << " address");
    return -1;
  }
  addr.sin_port = htons(port);

  // sendto should not block
  int result =
      sendto(m_lsd, reinterpret_cast<const char*>(data.data()), data.size(), 0,
             reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
  return result;
}

int UDPClient::send(StringRef data, const Twine& server, int port) {
  // server must be a resolvable IP address
  struct sockaddr_in addr;
  std::memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  SmallVector<char, 128> addr_store;
  StringRef remoteAddr = server.toNullTerminatedStringRef(addr_store);
  if (remoteAddr.empty()) {
    WPI_ERROR(m_logger, "server must be passed");
    return -1;
  }

#ifdef _WIN32
  int res = InetPton(AF_INET, remoteAddr.data(), &(addr.sin_addr));
#else
  int res = inet_pton(AF_INET, remoteAddr.data(), &(addr.sin_addr));
#endif
  if (res != 1) {
    WPI_ERROR(m_logger, "could not resolve " << server << " address");
    return -1;
  }
  addr.sin_port = htons(port);

  // sendto should not block
  int result = sendto(m_lsd, data.data(), data.size(), 0,
                      reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
  return result;
}

int UDPClient::receive(uint8_t* data_received, int receive_len) {
  if (m_port == 0) return -1;  // return if not receiving
  return recv(m_lsd, reinterpret_cast<char*>(data_received), receive_len, 0);
}

int UDPClient::receive(uint8_t* data_received, int receive_len,
                       SmallVectorImpl<char>* addr_received,
                       int* port_received) {
  if (m_port == 0) return -1;  // return if not receiving

  struct sockaddr_in remote;
  socklen_t remote_len = sizeof(remote);
  std::memset(&remote, 0, sizeof(remote));

  int result =
      recvfrom(m_lsd, reinterpret_cast<char*>(data_received), receive_len, 0,
               reinterpret_cast<sockaddr*>(&remote), &remote_len);

  char ip[50];
#ifdef _WIN32
  InetNtop(PF_INET, &(remote.sin_addr.s_addr), ip, sizeof(ip) - 1);
#else
  inet_ntop(PF_INET, reinterpret_cast<in_addr*>(&(remote.sin_addr.s_addr)), ip,
            sizeof(ip) - 1);
#endif

  ip[49] = '\0';
  int addr_len = std::strlen(ip);
  addr_received->clear();
  addr_received->append(&ip[0], &ip[addr_len]);

  *port_received = ntohs(remote.sin_port);

  return result;
}

int UDPClient::set_timeout(double timeout) {
  if (timeout < 0) return -1;
  struct timeval tv;
  tv.tv_sec = timeout;             // truncating will give seconds
  timeout -= tv.tv_sec;            // remove seconds portion
  tv.tv_usec = timeout * 1000000;  // fractions of a second to us
  int ret = setsockopt(m_lsd, SOL_SOCKET, SO_RCVTIMEO,
                       reinterpret_cast<char*>(&tv), sizeof(tv));
  if (ret < 0) WPI_ERROR(m_logger, "set timeout failed");
  return ret;
}
