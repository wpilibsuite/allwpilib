/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Utility.h"

#include "Timer.h"
#include "simulation/simTime.h"
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#if not defined(_WIN32)
	#include <execinfo.h>
	#include <cxxabi.h>
#endif

static bool stackTraceEnabled = false;
static bool suspendOnAssertEnabled = false;

/**
 * Enable Stack trace after asserts.
 */
void wpi_stackTraceOnAssertEnable(bool enabled)
{
	stackTraceEnabled = enabled;
}

/**
 * Enable suspend on asssert.
 * If enabled, the user task will be suspended whenever an assert fails. This
 * will allow the user to attach to the task with the debugger and examine variables
 * around the failure.
 */
void wpi_suspendOnAssertEnabled(bool enabled)
{
	suspendOnAssertEnabled = enabled;
}

static void wpi_handleTracing()
{
	// if (stackTraceEnabled)
	// {
	// 	printf("\n-----------<Stack Trace>----------------\n");
	// 	printCurrentStackTrace();
	// }
	printf("\n");
}

/**
 * Assert implementation.
 * This allows breakpoints to be set on an assert.
 * The users don't call this, but instead use the wpi_assert macros in Utility.h.
 */
bool wpi_assert_impl(bool conditionValue, const char *conditionText,
                     const char *message, const char *fileName,
                     uint32_t lineNumber, const char *funcName) {
  if (!conditionValue) {
    std::stringstream errorStream;

    errorStream << "Assertion \"" << conditionText << "\" ";
    errorStream << "on line " << lineNumber << " ";
    errorStream << "of " << basename(fileName) << " ";

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
 * This should not be called directly; it should only be used by wpi_assertEqual_impl
 * and wpi_assertNotEqual_impl.
 */
void wpi_assertEqual_common_impl(int valueA, int valueB,
                                 const std::string &equalityType,
                                 const std::string &message,
                                 const std::string &fileName,
                                 uint32_t lineNumber,
                                 const std::string &funcName) {
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
 * The users don't call this, but instead use the wpi_assertEqual macros in Utility.h.
 */
bool wpi_assertEqual_impl(int valueA,
					 	  int valueB,
						  const std::string &message,
						  const std::string &fileName,
						  uint32_t lineNumber,
						  const std::string &funcName)
{
	if(!(valueA == valueB))
	{
		wpi_assertEqual_common_impl(valueA, valueB, "!=", message, fileName, lineNumber, funcName);
	}
	return valueA == valueB;
}

/**
 * Assert not equal implementation.
 * This determines whether the two given integers are equal. If so,
 * the value of each is printed along with an optional message string.
 * The users don't call this, but instead use the wpi_assertNotEqual macros in Utility.h.
 */
bool wpi_assertNotEqual_impl(int valueA,
					 	     int valueB,
						     const std::string &message,
						     const std::string &fileName,
						     uint32_t lineNumber,
						     const std::string &funcName)
{
	if(!(valueA != valueB))
	{
		wpi_assertEqual_common_impl(valueA, valueB, "==", message, fileName, lineNumber, funcName);
	}
	return valueA != valueB;
}

/**
 * Read the microsecond-resolution timer on the FPGA.
 *
 * @return The current time in microseconds according to the FPGA (since FPGA reset).
 */
uint64_t GetFPGATime()
{
	return wpilib::internal::simTime * 1e6;
}

//TODO: implement symbol demangling and backtrace on windows
#if not defined(_WIN32)

/**
 * Demangle a C++ symbol, used for printing stack traces.
 */
static std::string demangle(char const *mangledSymbol)
{
	char buffer[256];
	size_t length;
	int status;

	if(sscanf(mangledSymbol, "%*[^(]%*[^_]%255[^)+]", buffer))
	{
		char *symbol = abi::__cxa_demangle(buffer, nullptr, &length, &status);

		if(status == 0)
		{
			return symbol;
		}
		else
		{
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
std::string GetStackTrace(uint32_t offset)
{
	void *stackTrace[128];
	int stackSize = backtrace(stackTrace, 128);
	char **mangledSymbols = backtrace_symbols(stackTrace, stackSize);
	std::stringstream trace;

	for(int i = offset; i < stackSize; i++)
	{
		// Only print recursive functions once in a row.
		if(i == 0 ||stackTrace[i] != stackTrace[i - 1])
		{
			trace << "\tat " << demangle(mangledSymbols[i]) << std::endl;
		}
	}

	free(mangledSymbols);

	return trace.str();
}

#else
static std::string demangle(char const *mangledSymbol)
{
	return "no demangling on windows";
}
std::string GetStackTrace(uint32_t offset)
{
	return "no stack trace on windows";
}
#endif
