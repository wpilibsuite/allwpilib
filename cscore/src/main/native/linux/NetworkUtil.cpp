// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "cscore_cpp.h"  // NOLINT(build/include_order)

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>
#include <vector>

namespace cs {

std::vector<std::string> GetNetworkInterfaces() {
  struct ifaddrs* ifa;
  if (::getifaddrs(&ifa) != 0) {
    return {};
  }

  std::vector<std::string> rv;
  char buf[256];
  for (struct ifaddrs* i = ifa; i; i = i->ifa_next) {
    if (!i->ifa_addr) {
      continue;  // no address
    }
    if (i->ifa_addr->sa_family != AF_INET) {
      continue;  // only return IPv4
    }
    struct sockaddr_in* addr_in = reinterpret_cast<sockaddr_in*>(i->ifa_addr);
    const char* addr =
        ::inet_ntop(addr_in->sin_family, &addr_in->sin_addr, buf, sizeof(buf));
    if (!addr) {
      continue;  // error converting address
    }
    rv.emplace_back(addr);
  }

  ::freeifaddrs(ifa);
  return rv;
}

}  // namespace cs
