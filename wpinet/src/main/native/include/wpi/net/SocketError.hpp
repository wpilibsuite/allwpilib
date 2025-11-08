// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_SOCKETERROR_HPP_
#define WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_SOCKETERROR_HPP_

#include <string>

namespace wpi {

int SocketErrno();

std::string SocketStrerror(int code);

inline std::string SocketStrerror() {
  return SocketStrerror(SocketErrno());
}

}  // namespace wpi

#endif  // WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_SOCKETERROR_HPP_
