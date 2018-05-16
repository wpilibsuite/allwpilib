/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_SOCKETERROR_H_
#define WPIUTIL_WPI_SOCKETERROR_H_

#include <string>

namespace wpi {

int SocketErrno();

std::string SocketStrerror(int code);

static inline std::string SocketStrerror() {
  return SocketStrerror(SocketErrno());
}

}  // namespace wpi

#endif  // WPIUTIL_WPI_SOCKETERROR_H_
