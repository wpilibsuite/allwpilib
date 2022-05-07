// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_SOCKETERROR_H_
#define WPINET_SOCKETERROR_H_

#include <string>

namespace wpi {

int SocketErrno();

std::string SocketStrerror(int code);

inline std::string SocketStrerror() {
  return SocketStrerror(SocketErrno());
}

}  // namespace wpi

#endif  // WPINET_SOCKETERROR_H_
