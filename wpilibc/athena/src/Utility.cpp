/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Utility.h"

#include <cxxabi.h>
#include <execinfo.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include "ErrorBase.h"
#include "HAL/DriverStation.h"
#include "HAL/HAL.h"
#include "llvm/SmallString.h"

using namespace frc;

/**
 * Assert implementation.
 * This allows breakpoints to be set on an assert.
 * The users don't call this, but instead use the wpi_assert macros in
 * Utility.h.
 */
bool wpi_assert_impl(bool conditionValue, llvm::StringRef conditionText,
                     llvm::StringRef message, llvm::StringRef fileName,
                     int lineNumber, llvm::StringRef funcName) {
  if (!conditionValue) {
    std::stringstream locStream;
    locStream << funcName << " [";
    llvm::SmallString<128> fileTemp;
    locStream << basename(fileName.c_str(fileTemp)) << ":" << lineNumber << "]";

    std::stringstream errorStream;

    errorStream << "Assertion \"" << conditionText << "\" ";

    if (message[0] != '\0') {
      errorStream << "failed: " << message << std::endl;
    } else {
      errorStream << "failed." << std::endl;
    }

    std::string stack = GetStackTrace(2);
    std::string location = locStream.str();
    std::string error = errorStream.str();

    // Print the error and send it to the DriverStation
    HAL_SendError(1, 1, 0, error.c_str(), location.c_str(), stack.c_str(), 1);
  }

  return conditionValue;
}

/**
 * Common error routines for wpi_assertEqual_impl and wpi_assertNotEqual_impl
 * This should not be called directly; it should only be used by
 * wpi_assertEqual_impl and wpi_assertNotEqual_impl.
 */
void wpi_assertEqual_common_impl(llvm::StringRef valueA, llvm::StringRef valueB,
                                 llvm::StringRef equalityType,
                                 llvm::StringRef message,
                                 llvm::StringRef fileName, int lineNumber,
                                 llvm::StringRef funcName) {
  std::stringstream locStream;
  locStream << funcName << " [";
  llvm::SmallString<128> fileTemp;
  locStream << basename(fileName.c_str(fileTemp)) << ":" << lineNumber << "]";

  std::stringstream errorStream;

  errorStream << "Assertion \"" << valueA << " " << equalityType << " "
              << valueB << "\" ";

  if (message[0] != '\0') {
    errorStream << "failed: " << message << std::endl;
  } else {
    errorStream << "failed." << std::endl;
  }

  std::string trace = GetStackTrace(3);
  std::string location = locStream.str();
  std::string error = errorStream.str();

  // Print the error and send it to the DriverStation
  HAL_SendError(1, 1, 0, error.c_str(), location.c_str(), trace.c_str(), 1);
}

/**
 * Assert equal implementation.
 * This determines whether the two given integers are equal. If not,
 * the value of each is printed along with an optional message string.
 * The users don't call this, but instead use the wpi_assertEqual macros in
 * Utility.h.
 */
bool wpi_assertEqual_impl(int valueA, int valueB, llvm::StringRef valueAString,
                          llvm::StringRef valueBString, llvm::StringRef message,
                          llvm::StringRef fileName, int lineNumber,
                          llvm::StringRef funcName) {
  if (!(valueA == valueB)) {
    wpi_assertEqual_common_impl(valueAString, valueBString, "==", message,
                                fileName, lineNumber, funcName);
  }
  return valueA == valueB;
}

/**
 * Assert not equal implementation.
 * This determines whether the two given integers are equal. If so,
 * the value of each is printed along with an optional message string.
 * The users don't call this, but instead use the wpi_assertNotEqual macros in
 * Utility.h.
 */
bool wpi_assertNotEqual_impl(int valueA, int valueB,
                             llvm::StringRef valueAString,
                             llvm::StringRef valueBString,
                             llvm::StringRef message, llvm::StringRef fileName,
                             int lineNumber, llvm::StringRef funcName) {
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
 * @return FPGA Version number.
 */
int GetFPGAVersion() {
  int32_t status = 0;
  int version = HAL_GetFPGAVersion(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return version;
}

/**
 * Return the FPGA Revision number.
 * The format of the revision is 3 numbers.
 * The 12 most significant bits are the Major Revision.
 * the next 8 bits are the Minor Revision.
 * The 12 least significant bits are the Build Number.
 * @return FPGA Revision number.
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
 */
bool GetUserButton() {
  int32_t status = 0;

  bool value = HAL_GetFPGAButton(&status);
  wpi_setGlobalError(status);

  return value;
}

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
 * @param offset The number of symbols at the top of the stack to ignore
 */
std::string GetStackTrace(int offset) {
  void* stackTrace[128];
  int stackSize = backtrace(stackTrace, 128);
  char** mangledSymbols = backtrace_symbols(stackTrace, stackSize);
  std::stringstream trace;

  for (int i = offset; i < stackSize; i++) {
    // Only print recursive functions once in a row.
    if (i == 0 || stackTrace[i] != stackTrace[i - 1]) {
      trace << "\tat " << demangle(mangledSymbols[i]) << std::endl;
    }
  }

  std::free(mangledSymbols);

  return trace.str();
}

}  // namespace frc
