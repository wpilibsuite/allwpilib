// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/uv/util.h"  // NOLINT(build/include_order)

#include <cstring>

#include "wpi/SmallString.h"

namespace wpi::uv {

int NameToAddr(const Twine& ip, unsigned int port, sockaddr_in* addr) {
  SmallString<128> tmp;
  StringRef ipStr = ip.toNullTerminatedStringRef(tmp);
  if (ipStr.empty()) {
    std::memset(addr, 0, sizeof(sockaddr_in));
    addr->sin_family = PF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(port);
    return 0;
  } else {
    return uv_ip4_addr(ipStr.data(), port, addr);
  }
}

int NameToAddr(const Twine& ip, unsigned int port, sockaddr_in6* addr) {
  SmallString<128> tmp;
  StringRef ipStr = ip.toNullTerminatedStringRef(tmp);
  if (ipStr.empty()) {
    std::memset(addr, 0, sizeof(sockaddr_in6));
    addr->sin6_family = PF_INET6;
    addr->sin6_addr = in6addr_any;
    addr->sin6_port = htons(port);
    return 0;
  } else {
    return uv_ip6_addr(ipStr.data(), port, addr);
  }
}

int NameToAddr(const Twine& ip, in_addr* addr) {
  SmallString<128> tmp;
  StringRef ipStr = ip.toNullTerminatedStringRef(tmp);
  if (ipStr.empty()) {
    addr->s_addr = INADDR_ANY;
    return 0;
  } else {
    return uv_inet_pton(AF_INET, ipStr.data(), addr);
  }
}

int NameToAddr(const Twine& ip, in6_addr* addr) {
  SmallString<128> tmp;
  StringRef ipStr = ip.toNullTerminatedStringRef(tmp);
  if (ipStr.empty()) {
    *addr = in6addr_any;
    return 0;
  } else {
    return uv_inet_pton(AF_INET6, ipStr.data(), addr);
  }
}

}  // namespace wpi::uv
