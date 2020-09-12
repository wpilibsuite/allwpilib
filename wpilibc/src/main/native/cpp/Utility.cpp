/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Utility.h"

#ifndef _WIN32
#include <cxxabi.h>
#include <execinfo.h>
#endif

#include <frc/Base.h>
#include <hal/DriverStation.h>
#include <wpi/Path.h>
#include <wpi/SmallString.h>
#include <wpi/StackTrace.h>
#include <wpi/raw_ostream.h>

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

    std::string stack = wpi::GetStackTrace(2);

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

  std::string trace = wpi::GetStackTrace(3);

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
