/*---------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							 */
/* Open Source Software - may be modified and shared by FRC teams. The code  */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib. */
/*---------------------------------------------------------------------------*/

#include <stdint.h>

#ifndef UTILITY_H_
#define UTILITY_H_

#define wpi_assert(condition) wpi_assert_impl(condition, #condition, NULL, __FILE__, __LINE__, __FUNCTION__)
#define wpi_assertWithMessage(condition, message) wpi_assert_impl(condition, #condition, message, __FILE__, __LINE__, __FUNCTION__)

#define wpi_assertEqual(a, b) wpi_assertEqual_impl(a, b, NULL, __FILE__, __LINE__, __FUNCTION__)
#define wpi_assertEqualWithMessage(a, b, message) wpi_assertEqual_impl(a, b, message, __FILE__, __LINE__, __FUNCTION__)

#define wpi_assertNotEqual(a, b) wpi_assertNotEqual_impl(a, b, NULL, __FILE__, __LINE__, __FUNCTION__)
#define wpi_assertNotEqualWithMessage(a, b, message) wpi_assertNotEqual_impl(a, b, message, __FILE__, __LINE__, __FUNCTION__)

bool wpi_assert_impl(bool conditionValue, const char *conditionText, const char *message, const char *fileName, uint32_t lineNumber, const char *funcName);
bool wpi_assertEqual_impl(int valueA, int valueB, const char *message, const char *fileName,uint32_t lineNumber, const char *funcName);
bool wpi_assertNotEqual_impl(int valueA, int valueB, const char *message, const char *fileName,uint32_t lineNumber, const char *funcName);

void wpi_suspendOnAssertEnabled(bool enabled);
void wpi_stackTraceOnAssertEnable(bool enabled);

uint32_t GetFPGATime();

#endif // UTILITY_H_
