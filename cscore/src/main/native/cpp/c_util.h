// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_C_UTIL_H_
#define CSCORE_C_UTIL_H_

#include <cstdlib>
#include <cstring>

#include <wpi/MemAlloc.h>
#include <wpi/StringRef.h>

namespace cs {

inline char* ConvertToC(wpi::StringRef in) {
  char* out = static_cast<char*>(wpi::safe_malloc(in.size() + 1));
  std::memmove(out, in.data(), in.size());
  out[in.size()] = '\0';
  return out;
}

}  // namespace cs

#endif  // CSCORE_C_UTIL_H_
