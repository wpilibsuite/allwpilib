/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "support/hostname.h"

#ifdef _WIN32
#include <Winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <unistd.h>
#endif

#include <string>

#include "llvm/SmallVector.h"
#include "llvm/StringRef.h"

#ifdef _WIN32
struct WSAHelper {
  WSAHelper() {
    WSAData wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);
  }
  ~WSAHelper() { WSACleanup(); }
};
static WSAHelper& GetWSAHelper() {
  static WSAHelper helper;
  return helper;
}
#endif

namespace wpi {
static bool GetHostnameImpl(char* name, size_t name_len) {
#ifdef _WIN32
  GetWSAHelper();
#endif
  if (::gethostname(name, name_len) != 0) return false;
  name[name_len - 1] =
      '\0';  // Per POSIX, may not be null terminated if too long
  return true;
}

std::string GetHostname() {
  char name[256];
  if (!GetHostnameImpl(name, sizeof(name))) return "";
  return name;
}

llvm::StringRef GetHostname(llvm::SmallVectorImpl<char>& name) {
  // Use a tmp array to not require the SmallVector to be too large.
  char tmpName[256];
  if (!GetHostnameImpl(tmpName, sizeof(tmpName))) {
    return llvm::StringRef{};
  }
  name.clear();
  name.append(tmpName, tmpName + std::strlen(tmpName) + 1);

  return llvm::StringRef{name.data(), name.size(), true};
}
}  // namespace wpi
