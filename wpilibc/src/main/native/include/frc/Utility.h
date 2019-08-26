/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
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

#include <wpi/StringRef.h>
#include <wpi/Twine.h>
#include <wpi/deprecated.h>

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

/**
 * Assert implementation.
 *
 * This allows breakpoints to be set on an assert. The users don't call this,
 * but instead use the wpi_assert macros in Utility.h.
 */
bool wpi_assert_impl(bool conditionValue, const wpi::Twine& conditionText,
                     const wpi::Twine& message, wpi::StringRef fileName,
                     int lineNumber, wpi::StringRef funcName);

/**
 * Assert equal implementation.
 *
 * This determines whether the two given integers are equal. If not, the value
 * of each is printed along with an optional message string. The users don't
 * call this, but instead use the wpi_assertEqual macros in Utility.h.
 */
bool wpi_assertEqual_impl(int valueA, int valueB,
                          const wpi::Twine& valueAString,
                          const wpi::Twine& valueBString,
                          const wpi::Twine& message, wpi::StringRef fileName,
                          int lineNumber, wpi::StringRef funcName);

/**
 * Assert not equal implementation.
 *
 * This determines whether the two given integers are equal. If so, the value of
 * each is printed along with an optional message string. The users don't call
 * this, but instead use the wpi_assertNotEqual macros in Utility.h.
 */
bool wpi_assertNotEqual_impl(int valueA, int valueB,
                             const wpi::Twine& valueAString,
                             const wpi::Twine& valueBString,
                             const wpi::Twine& message, wpi::StringRef fileName,
                             int lineNumber, wpi::StringRef funcName);
