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
  size_t n = str.size();
  char* write = WPI_AllocateString(output, n);
  if (n>0)
      std::memcpy(write, str.data(), n);
}

}  // namespace cs

#endif  // CSCORE_C_UTIL_H_
