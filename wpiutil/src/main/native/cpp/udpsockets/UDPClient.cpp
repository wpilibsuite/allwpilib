/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "udpsockets/UDPClient.h"

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

#include "support/Logger.h"
#include "tcpsockets/SocketError.h"

using namespace wpi;

UDPClient::UDPClient(Logger& logger) : UDPClient("", logger) {}

UDPClient::UDPClient(llvm::StringRef address, Logger& logger)
    : m_lsd(0), m_address(address), m_logger(logger) {}

UDPClient::UDPClient(UDPClient&& other)
    : m_lsd(other.m_lsd),
      m_address(std::move(other.m_address)),
      m_logger(other.m_logger) {
  other.m_lsd = 0;
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
  other.m_lsd = 0;
  return *this;
}

int UDPClient::start() {
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
    llvm::SmallString<128> addr_copy(m_address);
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
  addr.sin_port = htons(0);

  int result = bind(m_lsd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
  if (result != 0) {
    WPI_ERROR(m_logger, "bind() failed: " << SocketStrerror());
    return result;
  }
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
  }
}

int UDPClient::send(llvm::ArrayRef<uint8_t> data, llvm::StringRef server,
                    int port) {
  // server must be a resolvable IP address
  struct sockaddr_in addr;
  std::memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  if (server.size() > 0) {
    llvm::SmallVector<char, 128> addr_store;
    auto remoteAddr = server.c_str(addr_store);
#ifdef _WIN32
    int res = InetPton(AF_INET, remoteAddr, &(addr.sin_addr));
#else
    int res = inet_pton(AF_INET, remoteAddr, &(addr.sin_addr));
#endif
    if (res != 1) {
      WPI_ERROR(m_logger, "could not resolve " << server << " address");
      return -1;
    }
  } else {
    WPI_ERROR(m_logger, "server must be passed");
    return -1;
  }
  addr.sin_port = htons(port);

  // sendto should not block
  int result =
      sendto(m_lsd, reinterpret_cast<const char*>(data.data()), data.size(), 0,
             reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
  return result;
}

int UDPClient::send(llvm::StringRef data, llvm::StringRef server, int port) {
  // server must be a resolvable IP address
  struct sockaddr_in addr;
  std::memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  if (server.size() > 0) {
    llvm::SmallVector<char, 128> addr_store;
    auto remoteAddr = server.c_str(addr_store);
#ifdef _WIN32
    int res = InetPton(AF_INET, remoteAddr, &(addr.sin_addr));
#else
    int res = inet_pton(AF_INET, remoteAddr, &(addr.sin_addr));
#endif
    if (res != 1) {
      WPI_ERROR(m_logger, "could not resolve " << server << " address");
      return -1;
    }
  } else {
    WPI_ERROR(m_logger, "server must be passed");
    return -1;
  }
  addr.sin_port = htons(port);

  // sendto should not block
  int result = sendto(m_lsd, data.data(), data.size(), 0,
                      reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
  return result;
}
