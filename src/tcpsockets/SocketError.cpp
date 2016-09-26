/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "tcpsockets/SocketError.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <string.h>
#endif

namespace wpi {

std::string SocketStrerror(int code) {
#ifdef _WIN32
  LPSTR errstr = nullptr;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                0, code, 0, (LPSTR)&errstr, 0, 0);
  std::string rv(errstr);
  LocalFree(errstr);
  return rv;
#else
  return strerror(code);
#endif
}

}  // namespace wpi
