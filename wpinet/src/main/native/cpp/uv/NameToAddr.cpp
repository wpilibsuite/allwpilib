// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/uv/util.h"  // NOLINT(build/include_order)

#include <cstring>

#include <wpi/SmallString.h>

namespace wpi::uv {

int NameToAddr(std::string_view ip, unsigned int port, sockaddr_in* addr) {
  if (ip.empty()) {
    std::memset(addr, 0, sizeof(sockaddr_in));
    addr->sin_family = PF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(port);
    return 0;
  } else {
    SmallString<128> ipBuf{ip};
    return uv_ip4_addr(ipBuf.c_str(), port, addr);
  }
}

int NameToAddr(std::string_view ip, unsigned int port, sockaddr_in6* addr) {
  if (ip.empty()) {
    std::memset(addr, 0, sizeof(sockaddr_in6));
    addr->sin6_family = PF_INET6;
    addr->sin6_addr = in6addr_any;
    addr->sin6_port = htons(port);
    return 0;
  } else {
    SmallString<128> ipBuf{ip};
    return uv_ip6_addr(ipBuf.c_str(), port, addr);
  }
}

int NameToAddr(std::string_view ip, in_addr* addr) {
  if (ip.empty()) {
    addr->s_addr = INADDR_ANY;
    return 0;
  } else {
    SmallString<128> ipBuf{ip};
    return uv_inet_pton(AF_INET, ipBuf.c_str(), addr);
  }
}

int NameToAddr(std::string_view ip, in6_addr* addr) {
  if (ip.empty()) {
    *addr = in6addr_any;
    return 0;
  } else {
    SmallString<128> ipBuf{ip};
    return uv_inet_pton(AF_INET6, ipBuf.c_str(), addr);
  }
}

}  // namespace wpi::uv
