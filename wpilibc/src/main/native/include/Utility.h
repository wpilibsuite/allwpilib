/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

/** @file
 * Contains global utility functions
 */

#include <stdint.h>

#include <string>

#include <llvm/StringRef.h>

#define wpi_assert(condition) \
  wpi_assert_impl(condition, #condition, "", __FILE__, __LINE__, __FUNCTION__)
#define wpi_assertWithMessage(condition, message)                     \
  wpi_assert_impl(condition, #condition, message, __FILE__, __LINE__, \
                  __FUNCTION__)

#define wpi_assertEqual(a, b) \
  wpi_assertEqual_impl(a, b, #a, #b, "", __FILE__, __LINE__, __FUNCTION__)
#define wpi_assertEqualWithMessage(a, b, message) \
  wpi_assertEqual_impl(a, b, #a, #b, message, __FILE__, __LINE__, __FUNCTION__)

#define wpi_assertNotEqual(a, b) \
  wpi_assertNotEqual_impl(a, b, #a, #b, "", __FILE__, __LINE__, __FUNCTION__)
#define wpi_assertNotEqualWithMessage(a, b, message)                 \
  wpi_assertNotEqual_impl(a, b, #a, #b, message, __FILE__, __LINE__, \
                          __FUNCTION__)

bool wpi_assert_impl(bool conditionValue, llvm::StringRef conditionText,
                     llvm::StringRef message, llvm::StringRef fileName,
                     int lineNumber, llvm::StringRef funcName);
bool wpi_assertEqual_impl(int valueA, int valueB, llvm::StringRef valueAString,
                          llvm::StringRef valueBString, llvm::StringRef message,
                          llvm::StringRef fileName, int lineNumber,
                          llvm::StringRef funcName);
bool wpi_assertNotEqual_impl(int valueA, int valueB,
                             llvm::StringRef valueAString,
                             llvm::StringRef valueBString,
                             llvm::StringRef message, llvm::StringRef fileName,
                             int lineNumber, llvm::StringRef funcName);

void wpi_suspendOnAssertEnabled(bool enabled);

namespace frc {

int GetFPGAVersion();
int64_t GetFPGARevision();
uint64_t GetFPGATime();
bool GetUserButton();
std::string GetStackTrace(int offset);

}  // namespace frc
