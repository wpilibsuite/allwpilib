// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <uv.h>
#include <ws2tcpip.h>

#include <string>
#include <vector>

#include "cscore_cpp.h"

#pragma comment(lib, "Ws2_32.lib")

namespace cs {

std::vector<std::string> GetNetworkInterfaces() {
  uv_interface_address_t* adrs;
  int counts = 0;

  std::vector<std::string> addresses{};

  uv_interface_addresses(&adrs, &counts);

  char ip[50];

  for (int i = 0; i < counts; i++) {
    if (adrs[i].is_internal) {
      continue;
    }
    InetNtop(PF_INET, &(adrs[i].netmask.netmask4.sin_addr.s_addr), ip,
             sizeof(ip) - 1);
    ip[49] = '\0';
    InetNtop(PF_INET, &(adrs[i].address.address4.sin_addr.s_addr), ip,
             sizeof(ip) - 1);
    ip[49] = '\0';
    addresses.emplace_back(std::string{ip});
  }

  uv_free_interface_addresses(adrs, counts);

  return addresses;
}

}  // namespace cs
