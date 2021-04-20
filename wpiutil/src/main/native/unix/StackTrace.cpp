// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/StackTrace.h"

#include <execinfo.h>

#include "wpi/Demangle.h"
#include "wpi/SmallString.h"
#include "wpi/StringRef.h"
#include "wpi/raw_ostream.h"

namespace wpi {

std::string GetStackTrace(int offset) {
  void* stackTrace[128];
  int stackSize = backtrace(stackTrace, 128);
  char** mangledSymbols = backtrace_symbols(stackTrace, stackSize);
  wpi::SmallString<1024> buf;
  wpi::raw_svector_ostream trace(buf);

  for (int i = offset; i < stackSize; i++) {
    // Only print recursive functions once in a row.
    if (i == 0 || stackTrace[i] != stackTrace[i - 1]) {
      // extract just function name from "pathToExe(functionName+offset)"
      StringRef sym{mangledSymbols[i]};
      sym = sym.split('(').second;
      StringRef offset;
      std::tie(sym, offset) = sym.split('+');
      StringRef addr;
      std::tie(offset, addr) = offset.split(')');
      trace << "\tat " << Demangle(sym) << " + " << offset << addr << "\n";
    }
  }

  std::free(mangledSymbols);

  return trace.str();
}

}  // namespace wpi
