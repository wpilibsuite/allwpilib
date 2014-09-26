/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Utility.h"

#include "Task.h"
#include "Timer.h"
#include "simulation/simTime.h"
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <execinfo.h>
#include <cxxabi.h>

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
bool wpi_assert_impl(bool conditionValue, 
					 const char *conditionText,
					 const char *message,
					 const char *fileName,
					 uint32_t lineNumber, 
					 const char *funcName)
{
	if (!conditionValue)
	{   
		// Error string buffer
		char error[256];
				
		// If an error message was specified, include it
		// Build error string
		if(message != NULL) {
			sprintf(error, "Assertion failed: \"%s\", \"%s\" failed in %s() in %s at line %dd\n", 
							 message, conditionText, funcName, fileName, lineNumber);
		} else {
			sprintf(error, "Assertion failed: \"%s\" in %s() in %s at line %dd\n", 
							 conditionText, funcName, fileName, lineNumber);
		}
		
		// Print to console and send to remote dashboard
		printf("\n\n>>>>%s", error);
		
		wpi_handleTracing();
		if (suspendOnAssertEnabled) suspendTask(0);
	}
	return conditionValue;
}

/**
 * Common error routines for wpi_assertEqual_impl and wpi_assertNotEqual_impl
 * This should not be called directly; it should only be used by wpi_assertEqual_impl
 * and wpi_assertNotEqual_impl.
 */
void wpi_assertEqual_common_impl(int valueA,
					 	         int valueB,
					 	         const char *equalityType,
						         const char *message,
						         const char *fileName,
						         uint32_t lineNumber, 
						         const char *funcName)
{
	// Error string buffer
	char error[256];
			
	// If an error message was specified, include it
	// Build error string
	if(message != NULL) {
		sprintf(error, "Assertion failed: \"%s\", \"%d\" %s \"%d\" in %s() in %s at line %d\n", 
						 message, valueA, equalityType, valueB, funcName, fileName, lineNumber);
	} else {
		sprintf(error, "Assertion failed: \"%d\" %s \"%d\" in %s() in %s at line %d\n", 
						 valueA, equalityType, valueB, funcName, fileName, lineNumber);
	}
	
	// Print to console and send to remote dashboard
	printf("\n\n>>>>%s", error);
	
	wpi_handleTracing();
	if (suspendOnAssertEnabled) suspendTask(0);
}

/**
 * Assert equal implementation.
 * This determines whether the two given integers are equal. If not,
 * the value of each is printed along with an optional message string.
 * The users don't call this, but instead use the wpi_assertEqual macros in Utility.h.
 */
bool wpi_assertEqual_impl(int valueA,
					 	  int valueB,
						  const char *message,
						  const char *fileName,
						  uint32_t lineNumber, 
						  const char *funcName)
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
						     const char *message,
						     const char *fileName,
						     uint32_t lineNumber, 
						     const char *funcName)
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
uint32_t GetFPGATime()
{
	return wpilib::internal::simTime * 1e6;
}


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
		char *symbol = abi::__cxa_demangle(buffer, NULL, &length, &status);

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

