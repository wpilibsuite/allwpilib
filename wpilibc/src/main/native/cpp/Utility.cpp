/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Utility.h"

#ifndef _WIN32
#include <cxxabi.h>
#include <execinfo.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <llvm/Path.h>

#include "ErrorBase.h"
#include "HAL/DriverStation.h"
#include "HAL/HAL.h"
#include "llvm/SmallString.h"
#include "llvm/raw_ostream.h"

using namespace frc;

/**
 * Assert implementation.
 *
 * This allows breakpoints to be set on an assert. The users don't call this,
 * but instead use the wpi_assert macros in Utility.h.
 */
bool wpi_assert_impl(bool conditionValue, const llvm::Twine& conditionText,
                     const llvm::Twine& message, llvm::StringRef fileName,
                     int lineNumber, llvm::StringRef funcName) {
  if (!conditionValue) {
    llvm::SmallString<128> locBuf;
    llvm::raw_svector_ostream locStream(locBuf);
    locStream << funcName << " [" << llvm::sys::path::filename(fileName) << ":"
              << lineNumber << "]";

    llvm::SmallString<128> errorBuf;
    llvm::raw_svector_ostream errorStream(errorBuf);

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
void wpi_assertEqual_common_impl(const llvm::Twine& valueA,
                                 const llvm::Twine& valueB,
                                 const llvm::Twine& equalityType,
                                 const llvm::Twine& message,
                                 llvm::StringRef fileName, int lineNumber,
                                 llvm::StringRef funcName) {
  llvm::SmallString<128> locBuf;
  llvm::raw_svector_ostream locStream(locBuf);
  locStream << funcName << " [" << llvm::sys::path::filename(fileName) << ":"
            << lineNumber << "]";

  llvm::SmallString<128> errorBuf;
  llvm::raw_svector_ostream errorStream(errorBuf);

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

/**
 * Assert equal implementation.
 *
 * This determines whether the two given integers are equal. If not, the value
 * of each is printed along with an optional message string. The users don't
 * call this, but instead use the wpi_assertEqual macros in Utility.h.
 */
bool wpi_assertEqual_impl(int valueA, int valueB,
                          const llvm::Twine& valueAString,
                          const llvm::Twine& valueBString,
                          const llvm::Twine& message, llvm::StringRef fileName,
                          int lineNumber, llvm::StringRef funcName) {
  if (!(valueA == valueB)) {
    wpi_assertEqual_common_impl(valueAString, valueBString, "==", message,
                                fileName, lineNumber, funcName);
  }
  return valueA == valueB;
}

/**
 * Assert not equal implementation.
 *
 * This determines whether the two given integers are equal. If so, the value of
 * each is printed along with an optional message string. The users don't call
 * this, but instead use the wpi_assertNotEqual macros in Utility.h.
 */
bool wpi_assertNotEqual_impl(int valueA, int valueB,
                             const llvm::Twine& valueAString,
                             const llvm::Twine& valueBString,
                             const llvm::Twine& message,
                             llvm::StringRef fileName, int lineNumber,
                             llvm::StringRef funcName) {
  if (!(valueA != valueB)) {
    wpi_assertEqual_common_impl(valueAString, valueBString, "!=", message,
                                fileName, lineNumber, funcName);
  }
  return valueA != valueB;
}

namespace frc {

/**
 * Return the FPGA Version number.
 *
 * For now, expect this to be competition year.
 *
 * @return FPGA Version number.
 * @deprecated Use RobotController static class method
 */
int GetFPGAVersion() {
  int32_t status = 0;
  int version = HAL_GetFPGAVersion(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return version;
}

/**
 * Return the FPGA Revision number.
 *
 * The format of the revision is 3 numbers. The 12 most significant bits are the
 * Major Revision. The next 8 bits are the Minor Revision. The 12 least
 * significant bits are the Build Number.
 *
 * @return FPGA Revision number.
 * @deprecated Use RobotController static class method
 */
int64_t GetFPGARevision() {
  int32_t status = 0;
  int64_t revision = HAL_GetFPGARevision(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return revision;
}

/**
 * Read the microsecond-resolution timer on the FPGA.
 *
 * @return The current time in microseconds according to the FPGA (since FPGA
 *         reset).
 * @deprecated Use RobotController static class method
 */
uint64_t GetFPGATime() {
  int32_t status = 0;
  uint64_t time = HAL_GetFPGATime(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return time;
}

/**
 * Get the state of the "USER" button on the roboRIO.
 *
 * @return True if the button is currently pressed down
 * @deprecated Use RobotController static class method
 */
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

/**
 * Get a stack trace, ignoring the first "offset" symbols.
 *
 * @param offset The number of symbols at the top of the stack to ignore
 */
std::string GetStackTrace(int offset) {
  void* stackTrace[128];
  int stackSize = backtrace(stackTrace, 128);
  char** mangledSymbols = backtrace_symbols(stackTrace, stackSize);
  llvm::SmallString<1024> buf;
  llvm::raw_svector_ostream trace(buf);

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
