/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Utility.h"

#ifndef _WIN32
#include <cxxabi.h>
#include <execinfo.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <hal/DriverStation.h>
#include <hal/HAL.h>
#include <wpi/Path.h>
#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#include "frc/ErrorBase.h"

using namespace frc;

bool wpi_assert_impl(bool conditionValue, const wpi::Twine& conditionText,
                     const wpi::Twine& message, wpi::StringRef fileName,
                     int lineNumber, wpi::StringRef funcName) {
  if (!conditionValue) {
    wpi::SmallString<128> locBuf;
    wpi::raw_svector_ostream locStream(locBuf);
    locStream << funcName << " [" << wpi::sys::path::filename(fileName) << ":"
              << lineNumber << "]";

    wpi::SmallString<128> errorBuf;
    wpi::raw_svector_ostream errorStream(errorBuf);

    errorStream << "Assertion \"" << conditionText << "\" ";

    if (message.isTriviallyEmpty() ||
        (message.isSingleStringRef() && message.getSingleStringRef().empty())) {
      errorStream << "failed.\n";
    } else {
      errorStream << "failed: " << message << "\n";
    }

    std::string stack = GetStackTrace(2);

    // Print the error and send it to the DriverStation
    HAL_SendError(1, 1, 0, errorBuf.c_str(), locBuf.c_str(), stack.c_str(), 1);
  }

  return conditionValue;
}

/**
 * Common error routines for wpi_assertEqual_impl and wpi_assertNotEqual_impl.
 *
 * This should not be called directly; it should only be used by
 * wpi_assertEqual_impl and wpi_assertNotEqual_impl.
 */
void wpi_assertEqual_common_impl(const wpi::Twine& valueA,
                                 const wpi::Twine& valueB,
                                 const wpi::Twine& equalityType,
                                 const wpi::Twine& message,
                                 wpi::StringRef fileName, int lineNumber,
                                 wpi::StringRef funcName) {
  wpi::SmallString<128> locBuf;
  wpi::raw_svector_ostream locStream(locBuf);
  locStream << funcName << " [" << wpi::sys::path::filename(fileName) << ":"
            << lineNumber << "]";

  wpi::SmallString<128> errorBuf;
  wpi::raw_svector_ostream errorStream(errorBuf);

  errorStream << "Assertion \"" << valueA << " " << equalityType << " "
              << valueB << "\" ";

  if (message.isTriviallyEmpty() ||
      (message.isSingleStringRef() && message.getSingleStringRef().empty())) {
    errorStream << "failed.\n";
  } else {
    errorStream << "failed: " << message << "\n";
  }

  std::string trace = GetStackTrace(3);

  // Print the error and send it to the DriverStation
  HAL_SendError(1, 1, 0, errorBuf.c_str(), locBuf.c_str(), trace.c_str(), 1);
}

bool wpi_assertEqual_impl(int valueA, int valueB,
                          const wpi::Twine& valueAString,
                          const wpi::Twine& valueBString,
                          const wpi::Twine& message, wpi::StringRef fileName,
                          int lineNumber, wpi::StringRef funcName) {
  if (!(valueA == valueB)) {
    wpi_assertEqual_common_impl(valueAString, valueBString, "==", message,
                                fileName, lineNumber, funcName);
  }
  return valueA == valueB;
}

bool wpi_assertNotEqual_impl(int valueA, int valueB,
                             const wpi::Twine& valueAString,
                             const wpi::Twine& valueBString,
                             const wpi::Twine& message, wpi::StringRef fileName,
                             int lineNumber, wpi::StringRef funcName) {
  if (!(valueA != valueB)) {
    wpi_assertEqual_common_impl(valueAString, valueBString, "!=", message,
                                fileName, lineNumber, funcName);
  }
  return valueA != valueB;
}

namespace frc {

int GetFPGAVersion() {
  int32_t status = 0;
  int version = HAL_GetFPGAVersion(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return version;
}

int64_t GetFPGARevision() {
  int32_t status = 0;
  int64_t revision = HAL_GetFPGARevision(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return revision;
}

uint64_t GetFPGATime() {
  int32_t status = 0;
  uint64_t time = HAL_GetFPGATime(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return time;
}

bool GetUserButton() {
  int32_t status = 0;

  bool value = HAL_GetFPGAButton(&status);
  wpi_setGlobalError(status);

  return value;
}

#ifndef _WIN32

/**
 * Demangle a C++ symbol, used for printing stack traces.
 */
static std::string demangle(char const* mangledSymbol) {
  char buffer[256];
  size_t length;
  int32_t status;

  if (std::sscanf(mangledSymbol, "%*[^(]%*[(]%255[^)+]", buffer)) {
    char* symbol = abi::__cxa_demangle(buffer, nullptr, &length, &status);
    if (status == 0) {
      return symbol;
    } else {
      // If the symbol couldn't be demangled, it's probably a C function,
      // so just return it as-is.
      return buffer;
    }
  }

  // If everything else failed, just return the mangled symbol
  return mangledSymbol;
}

std::string GetStackTrace(int offset) {
  void* stackTrace[128];
  int stackSize = backtrace(stackTrace, 128);
  char** mangledSymbols = backtrace_symbols(stackTrace, stackSize);
  wpi::SmallString<1024> buf;
  wpi::raw_svector_ostream trace(buf);

  for (int i = offset; i < stackSize; i++) {
    // Only print recursive functions once in a row.
    if (i == 0 || stackTrace[i] != stackTrace[i - 1]) {
      trace << "\tat " << demangle(mangledSymbols[i]) << "\n";
    }
  }

  std::free(mangledSymbols);

  return trace.str();
}

#else
static std::string demangle(char const* mangledSymbol) {
  return "no demangling on windows";
}

std::string GetStackTrace(int offset) { return "no stack trace on windows"; }
#endif

}  // namespace frc
