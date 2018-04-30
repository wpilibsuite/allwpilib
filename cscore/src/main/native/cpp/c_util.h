/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef ALLWPILIB_C_UTIL_H_
#define ALLWPILIB_C_UTIL_H_

#include <cstdlib>
#include <cstring>

#include <llvm/StringRef.h>

namespace cs {

inline char* ConvertToC(llvm::StringRef in) {
  char* out = static_cast<char*>(std::malloc(in.size() + 1));
  std::memmove(out, in.data(), in.size());
  out[in.size()] = '\0';
  return out;
}

}  // namespace cs

#endif  // ALLWPILIB_C_UTIL_H_
