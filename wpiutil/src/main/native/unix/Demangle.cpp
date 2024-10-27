// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/Demangle.h"

#include <cxxabi.h>

#include <cstdio>
#include <string>

#include "wpi/SmallString.h"

namespace wpi {

std::string Demangle(std::string_view mangledSymbol) {
  SmallString<128> buf{mangledSymbol};
  size_t length;
  int32_t status;

  char* symbol = abi::__cxa_demangle(buf.c_str(), nullptr, &length, &status);
  if (status == 0) {
    std::string rv{symbol};
    std::free(symbol);
    return rv;
  }

  // If everything else failed, just return the mangled symbol
  return std::string{mangledSymbol};
}

}  // namespace wpi
