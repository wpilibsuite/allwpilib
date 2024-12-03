// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/StackTrace.h"

#include <execinfo.h>

#include <string>

#include "wpi/Demangle.h"
#include "wpi/SmallString.h"
#include "wpi/StringExtras.h"
#include "wpi/raw_ostream.h"

namespace wpi {

std::string GetStackTraceDefault(int offset) {
#ifndef __ANDROID__
  void* stackTrace[128];
  int stackSize = backtrace(stackTrace, 128);
  char** mangledSymbols = backtrace_symbols(stackTrace, stackSize);
  wpi::SmallString<1024> buf;
  wpi::raw_svector_ostream trace(buf);

  for (int i = offset; i < stackSize; i++) {
    // Only print recursive functions once in a row.
    if (i == 0 || stackTrace[i] != stackTrace[i - 1]) {
      // extract just function name from "pathToExe(functionName+offset)"
      std::string_view sym = split(mangledSymbols[i], '(').second;
      std::string_view offset;
      std::tie(sym, offset) = split(sym, '+');
      std::string_view addr;
      std::tie(offset, addr) = split(offset, ')');
      trace << "\tat " << Demangle(sym) << " + " << offset << addr << "\n";
    }
  }

  std::free(mangledSymbols);

  return std::string{trace.str()};
#else
  // backtrace_symbols not supported on android
  return "";
#endif
}

}  // namespace wpi
