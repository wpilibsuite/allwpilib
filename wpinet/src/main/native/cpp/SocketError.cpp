// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/SocketError.h"

#include <string>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <cerrno>
#include <cstring>
#endif

namespace wpi {

int SocketErrno() {
#ifdef _WIN32
  return WSAGetLastError();
#else
  return errno;
#endif
}

std::string SocketStrerror(int code) {
#ifdef _WIN32
  LPSTR errstr = nullptr;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0,
                code, 0, (LPSTR)&errstr, 0, 0);
  std::string rv(errstr);
  LocalFree(errstr);
  return rv;
#else
  return std::strerror(code);
#endif
}

}  // namespace wpi
