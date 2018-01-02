/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

/**
 * @file Contains global utility functions
 */

#include <stdint.h>

#include <string>

#include <llvm/StringRef.h>
#include <llvm/Twine.h>
#include <support/deprecated.h>

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

bool wpi_assert_impl(bool conditionValue, const llvm::Twine& conditionText,
                     const llvm::Twine& message, llvm::StringRef fileName,
                     int lineNumber, llvm::StringRef funcName);
bool wpi_assertEqual_impl(int valueA, int valueB,
                          const llvm::Twine& valueAString,
                          const llvm::Twine& valueBString,
                          const llvm::Twine& message, llvm::StringRef fileName,
                          int lineNumber, llvm::StringRef funcName);
bool wpi_assertNotEqual_impl(int valueA, int valueB,
                             const llvm::Twine& valueAString,
                             const llvm::Twine& valueBString,
                             const llvm::Twine& message,
                             llvm::StringRef fileName, int lineNumber,
                             llvm::StringRef funcName);

void wpi_suspendOnAssertEnabled(bool enabled);

namespace frc {

WPI_DEPRECATED("Use RobotController static class method")
int GetFPGAVersion();
WPI_DEPRECATED("Use RobotController static class method")
int64_t GetFPGARevision();
WPI_DEPRECATED("Use RobotController static class method")
uint64_t GetFPGATime();
WPI_DEPRECATED("Use RobotController static class method")
bool GetUserButton();
std::string GetStackTrace(int offset);

}  // namespace frc
