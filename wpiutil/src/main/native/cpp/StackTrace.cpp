// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/StackTrace.hpp"

#ifdef __cpp_lib_stacktrace
#include <stacktrace>
#elif !defined(_WIN32) && !defined(__EMSCRIPTEN__)
#include <execinfo.h>
#endif

#include <string>

#include "wpi/util/Demangle.hpp"
#include "wpi/util/SmallString.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/raw_ostream.hpp"

namespace wpi::util {

std::string GetStackTraceDefault(int offset) {
  wpi::util::SmallString<1024> buf;
  wpi::util::raw_svector_ostream trace(buf);

#ifdef __cpp_lib_stacktrace
  auto stackTrace = std::stacktrace::current();

  for (size_t i = offset; i < stackTrace.size(); ++i) {
    // Only print recursive functions once in a row
    if (i == 0 || stackTrace[i] != stackTrace[i - 1]) {
      trace << "\tat " << std::to_string(stackTrace[i]) << '\n';
    }
  }

  return std::string{trace.str()};
#elif !defined(_WIN32) && !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)
  void* stackTrace[128];
  int stackSize = backtrace(stackTrace, 128);
  char** mangledSymbols = backtrace_symbols(stackTrace, stackSize);

  for (int i = offset; i < stackSize; ++i) {
    // Only print recursive functions once in a row
    if (i == 0 || stackTrace[i] != stackTrace[i - 1]) {
      // Extract just function name from "pathToExe(functionName+offset)"
      std::string_view sym = split(mangledSymbols[i], '(').second;
      std::string_view offset;
      std::tie(sym, offset) = split(sym, '+');
      std::string_view addr;
      std::tie(offset, addr) = split(offset, ')');

      trace << "\tat " << Demangle(sym) << " + " << offset << addr << '\n';
    }
  }

  std::free(mangledSymbols);

  return std::string{trace.str()};
#else
  // backtrace_symbols not supported
  return "";
#endif
}

}  // namespace wpi::util
