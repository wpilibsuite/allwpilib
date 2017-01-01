/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
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
#include <iostream>
#include <sstream>

#include "Timer.h"
#include "llvm/SmallString.h"
#include "simulation/simTime.h"

using namespace frc;

static bool stackTraceEnabled = false;
static bool suspendOnAssertEnabled = false;

/**
 * Enable Stack trace after asserts.
 */
void wpi_stackTraceOnAssertEnable(bool enabled) { stackTraceEnabled = enabled; }

/**
 * Enable suspend on asssert.
 *
 * If enabled, the user task will be suspended whenever an assert fails. This
 * will allow the user to attach to the task with the debugger and examine
 * variables around the failure.
 */
void wpi_suspendOnAssertEnabled(bool enabled) {
  suspendOnAssertEnabled = enabled;
}

static void wpi_handleTracing() {
  // if (stackTraceEnabled)
  // {
  //   std::printf("\n-----------<Stack Trace>----------------\n");
  //   printCurrentStackTrace();
  // }
  std::printf("\n");
}

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
    std::stringstream errorStream;

    errorStream << "Assertion \"" << conditionText << "\" ";
    errorStream << "on line " << lineNumber << " ";

    llvm::SmallString<128> fileTemp;
    errorStream << "of " << basename(fileName.c_str(fileTemp)) << " ";

    if (message[0] != '\0') {
      errorStream << "failed: " << message << std::endl;
    } else {
      errorStream << "failed." << std::endl;
    }

    // Print to console and send to remote dashboard
    std::cout << "\n\n>>>>" << errorStream.str();
    wpi_handleTracing();
  }

  return conditionValue;
}

/**
 * Common error routines for wpi_assertEqual_impl and wpi_assertNotEqual_impl
 * This should not be called directly; it should only be used by
 * wpi_assertEqual_impl and wpi_assertNotEqual_impl.
 */
void wpi_assertEqual_common_impl(int valueA, int valueB,
                                 llvm::StringRef equalityType,
                                 llvm::StringRef message,
                                 llvm::StringRef fileName, int lineNumber,
                                 llvm::StringRef funcName) {
  // Error string buffer
  std::stringstream error;

  // If an error message was specified, include it
  // Build error string
  if (message.size() > 0) {
    error << "Assertion failed: \"" << message << "\", \"" << valueA << "\" "
          << equalityType << " \"" << valueB << "\" in " << funcName << "() in "
          << fileName << " at line " << lineNumber << "\n";
  } else {
    error << "Assertion failed: \"" << valueA << "\" " << equalityType << " \""
          << valueB << "\" in " << funcName << "() in " << fileName
          << " at line " << lineNumber << "\n";
  }

  // Print to console and send to remote dashboard
  std::cout << "\n\n>>>>" << error.str();

  wpi_handleTracing();
}

/**
 * Assert equal implementation.
 * This determines whether the two given integers are equal. If not,
 * the value of each is printed along with an optional message string.
 * The users don't call this, but instead use the wpi_assertEqual macros in
 * Utility.h.
 */
bool wpi_assertEqual_impl(int valueA, int valueB, llvm::StringRef message,
                          llvm::StringRef fileName, int lineNumber,
                          llvm::StringRef funcName) {
  if (!(valueA == valueB)) {
    wpi_assertEqual_common_impl(valueA, valueB, "!=", message, fileName,
                                lineNumber, funcName);
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
bool wpi_assertNotEqual_impl(int valueA, int valueB, llvm::StringRef message,
                             llvm::StringRef fileName, int lineNumber,
                             llvm::StringRef funcName) {
  if (!(valueA != valueB)) {
    wpi_assertEqual_common_impl(valueA, valueB, "==", message, fileName,
                                lineNumber, funcName);
  }
  return valueA != valueB;
}

namespace frc {

/**
 * Read the microsecond-resolution timer on the FPGA.
 *
 * @return The current time in microseconds according to the FPGA (since FPGA
 * reset).
 */
uint64_t GetFPGATime() { return wpilib::internal::simTime * 1e6; }

// TODO: implement symbol demangling and backtrace on windows
#if not defined(_WIN32)

/**
 * Demangle a C++ symbol, used for printing stack traces.
 */
static std::string demangle(char const* mangledSymbol) {
  char buffer[256];
  size_t length;
  int32_t status;

  if (std::sscanf(mangledSymbol, "%*[^(]%*[^_]%255[^)+]", buffer)) {
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

#else
static std::string demangle(char const* mangledSymbol) {
  return "no demangling on windows";
}
std::string GetStackTrace(int offset) { return "no stack trace on windows"; }
#endif

}  // namespace frc
