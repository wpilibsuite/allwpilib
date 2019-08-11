/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/StackTrace.h"

#ifndef _WIN32
#include <execinfo.h>
#endif

#include "wpi/Demangle.h"
#include "wpi/SmallString.h"
#include "wpi/raw_ostream.h"

namespace wpi {

#ifndef _WIN32

std::string GetStackTrace(int offset) {
  void* stackTrace[128];
  int stackSize = backtrace(stackTrace, 128);
  char** mangledSymbols = backtrace_symbols(stackTrace, stackSize);
  wpi::SmallString<1024> buf;
  wpi::raw_svector_ostream trace(buf);

  for (int i = offset; i < stackSize; i++) {
    // Only print recursive functions once in a row.
    if (i == 0 || stackTrace[i] != stackTrace[i - 1]) {
      trace << "\tat " << Demangle(mangledSymbols[i]) << "\n";
    }
  }

  std::free(mangledSymbols);

  return trace.str();
}

#else

std::string GetStackTrace(int offset) { return "no stack trace on windows"; }

#endif

}  // namespace wpi
