// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/Demangle.h"

#include <windows.h>  // NOLINT(build/include_order)

#include <dbghelp.h>

#include <string>

#include "wpi/SmallString.h"
#include "wpi/mutex.h"

#pragma comment(lib, "Dbghelp.lib")

namespace wpi {

std::string Demangle(std::string_view mangledSymbol) {
  static wpi::mutex m;
  std::scoped_lock lock(m);
  SmallString<128> buf{mangledSymbol};
  char buffer[256];
  DWORD sz = UnDecorateSymbolName(buf.c_str(), buffer, sizeof(buffer),
                                  UNDNAME_COMPLETE);
  if (sz == 0) {
    return std::string{mangledSymbol};
  }
  return std::string(buffer, sz);
}

}  // namespace wpi
