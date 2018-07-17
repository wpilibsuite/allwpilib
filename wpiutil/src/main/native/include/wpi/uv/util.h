/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_UTIL_H_
#define WPIUTIL_WPI_UV_UTIL_H_

#include <uv.h>

#include <cstring>

#include "wpi/Twine.h"

namespace wpi {
namespace uv {

/**
 * Convert a binary structure containing an IPv4 address to a string.
 * @param addr Binary structure
 * @param ip Output string (any type that has `assign(char*, char*)`)
 * @param port Output port number
 * @return Error (same as `uv_ip4_name()`).
 */
template <typename T>
int AddrToName(const sockaddr_in& addr, T* ip, unsigned int* port) {
  char name[128];
  int err = uv_ip4_name(&addr, name, 128);
  if (err == 0) {
    ip->assign(name, name + std::strlen(name));
    *port = ntohs(addr.sin_port);
  } else {
    ip->assign(name, name);
  }
  return err;
}

/**
 * Convert a binary structure containing an IPv6 address to a string.
 * @param addr Binary structure
 * @param ip Output string (any type that has `assign(char*, char*)`)
 * @param port Output port number
 * @return Error (same as `uv_ip6_name()`).
 */
template <typename T>
int AddrToName(const sockaddr_in6& addr, T* ip, unsigned int* port) {
  char name[128];
  int err = uv_ip6_name(&addr, name, 128);
  if (err == 0) {
    ip->assign(name, name + std::strlen(name));
    *port = ntohs(addr.sin6_port);
  } else {
    ip->assign(name, name);
  }
  return err;
}

/**
 * Convert a binary IPv4 address to a string.
 * @param addr Binary address
 * @param ip Output string (any type that has `assign(char*, char*)`)
 * @return Error (same as `uv_inet_ntop()`).
 */
template <typename T>
int AddrToName(const in_addr& addr, T* ip) {
  char name[128];
  int err = uv_inet_ntop(AF_INET, &addr, name, 128);
  if (err == 0)
    ip->assign(name, name + std::strlen(name));
  else
    ip->assign(name, name);
  return err;
}

/**
 * Convert a binary IPv6 address to a string.
 * @param addr Binary address
 * @param ip Output string (any type that has `assign(char*, char*)`)
 * @return Error (same as `uv_inet_ntop()`).
 */
template <typename T>
int AddrToName(const in6_addr& addr, T* ip) {
  char name[128];
  int err = uv_inet_ntop(AF_INET6, &addr, name, 128);
  if (err == 0)
    ip->assign(name, name + std::strlen(name));
  else
    ip->assign(name, name);
  return err;
}

/**
 * Convert a string containing an IPv4 address to a binary structure.
 * @param ip IPv4 address string
 * @param port Port number
 * @param addr Output binary structure
 * @return Error (same as `uv_ip4_addr()`).
 */
int NameToAddr(const Twine& ip, unsigned int port, sockaddr_in* addr);

/**
 * Convert a string containing an IPv6 address to a binary structure.
 * @param ip IPv6 address string
 * @param port Port number
 * @param addr Output binary structure
 * @return Error (same as `uv_ip6_addr()`).
 */
int NameToAddr(const Twine& ip, unsigned int port, sockaddr_in6* addr);

/**
 * Convert a string containing an IPv4 address to binary format.
 * @param ip IPv4 address string
 * @param addr Output binary
 * @return Error (same as `uv_inet_pton()`).
 */
int NameToAddr(const Twine& ip, in_addr* addr);

/**
 * Convert a string containing an IPv6 address to binary format.
 * @param ip IPv6 address string
 * @param addr Output binary
 * @return Error (same as `uv_inet_pton()`).
 */
int NameToAddr(const Twine& ip, in6_addr* addr);

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_UTIL_H_
