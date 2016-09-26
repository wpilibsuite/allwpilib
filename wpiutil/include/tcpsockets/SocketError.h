/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_TCPSOCKETS_SOCKETERROR_H_
#define WPIUTIL_TCPSOCKETS_SOCKETERROR_H_

#include <string>

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <errno.h>
#endif

namespace wpi {

static inline int SocketErrno() {
#ifdef _WIN32
  return WSAGetLastError();
#else
  return errno;
#endif
}

std::string SocketStrerror(int code);

static inline std::string SocketStrerror() {
  return SocketStrerror(SocketErrno());
}

}  // namespace wpi

#endif  // WPIUTIL_TCPSOCKETS_SOCKETERROR_H_
