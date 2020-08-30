/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/Demangle.h"

#include <cxxabi.h>

#include <cstdio>

#include "wpi/SmallString.h"

namespace wpi {

std::string Demangle(const Twine& mangledSymbol) {
  SmallString<128> buf;
  size_t length;
  int32_t status;

  char* symbol =
      abi::__cxa_demangle(mangledSymbol.toNullTerminatedStringRef(buf).data(),
                          nullptr, &length, &status);
  if (status == 0) {
    std::string rv{symbol};
    std::free(symbol);
    return rv;
  }

  // If everything else failed, just return the mangled symbol
  return mangledSymbol.str();
}

}  // namespace wpi
