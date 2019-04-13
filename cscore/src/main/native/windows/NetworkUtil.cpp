/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <uv.h>

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
    if (adrs[i].is_internal) continue;
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
