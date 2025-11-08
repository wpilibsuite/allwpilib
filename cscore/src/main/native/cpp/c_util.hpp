// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_C_UTIL_H_
#define CSCORE_C_UTIL_H_

#include <cstring>
#include <string_view>

#include <wpi/string.h>

namespace cs {

inline void ConvertToC(struct WPI_String* output, std::string_view str) {
  char* write = WPI_AllocateString(output, str.size());
  std::memcpy(write, str.data(), str.size());
}

}  // namespace cs

#endif  // CSCORE_C_UTIL_H_
