/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <winsock2.h>  // NOLINT(build/include_order)

#include <iphlpapi.h>  // NOLINT(build/include_order)

#include <ws2tcpip.h>  // NOLINT(build/include_order)

#include <uv.h>

#include <iostream>

#include "cscore_cpp.h"

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Ws2_32.lib")

#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

namespace cs {

std::vector<std::string> GetNetworkInterfaces() {
  uv_interface_address_t* adrs;
  int counts = 0;

  std::vector<std::string> addresses{};

  uv_interface_addresses(&adrs, &counts);

  char ip[50];

  for (int i = 0; i < counts; i++) {
    if (adrs[i].is_internal) continue;
    std::cout << adrs[i].name << std::endl;
    InetNtop(PF_INET, &(adrs[i].netmask.netmask4.sin_addr.s_addr), ip,
             sizeof(ip) - 1);
    ip[49] = '\0';
    std::cout << ip << std::endl;
    InetNtop(PF_INET, &(adrs[i].address.address4.sin_addr.s_addr), ip,
             sizeof(ip) - 1);
    ip[49] = '\0';
    std::cout << ip << std::endl;
    addresses.emplace_back(std::string{ip});
  }

  uv_free_interface_addresses(adrs, counts);

  std::cout << "finished\n";

  return addresses;

  PIP_ADAPTER_ADDRESSES pAddresses = NULL;
  PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
  PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
  PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
  PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
  unsigned int i = 0;

  ULONG outBufLen = 0;
  ULONG Iterations = 0;
  DWORD dwRetVal = 0;

  // Allocate a 15 KB buffer to start with.
  outBufLen = WORKING_BUFFER_SIZE;

  do {
    pAddresses = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(MALLOC(outBufLen));
    if (pAddresses == NULL) {
      std::printf("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
      std::exit(1);
    }

    dwRetVal = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL,
                                    pAddresses, &outBufLen);

    if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
      FREE(pAddresses);
      pAddresses = NULL;
    } else {
      break;
    }

    Iterations++;
  } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

  if (dwRetVal == NO_ERROR) {
    pCurrAddresses = pAddresses;
    while (pCurrAddresses) {
      pUnicast = pCurrAddresses->FirstUnicastAddress;
      while (pUnicast != NULL) {
        sockaddr_in* address =
            reinterpret_cast<sockaddr_in*>(pUnicast->Address.lpSockaddr);
        InetNtop(PF_INET, &(address->sin_addr.s_addr), ip, sizeof(ip) - 1);
        ip[49] = '\0';
        addresses.emplace_back(std::string{ip});
        pUnicast = pUnicast->Next;
      }

      pCurrAddresses = pCurrAddresses->Next;
    }
  }

  if (pAddresses) {
    FREE(pAddresses);
  }

  return addresses;  // TODO
}

}  // namespace cs
