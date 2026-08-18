// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hal/DriverStation.h"
#include "wpi/hal/DriverStationTypes.hpp"
#include "wpi/util/string.hpp"

namespace wpi::hal {
inline int32_t SendError(bool isError, int32_t errorCode,
                         std::string_view details, std::string_view location,
                         std::string_view callStack, bool printMsg) {
  WPI_String detailsWpiStr = wpi::util::make_string(details);
  WPI_String locationWpiStr = wpi::util::make_string(location);
  WPI_String callStackWpiStr = wpi::util::make_string(callStack);
  return HAL_SendError(isError, errorCode, &detailsWpiStr, &locationWpiStr,
                       &callStackWpiStr, printMsg);
}

inline int32_t SendConsoleLine(std::string_view line) {
  WPI_String lineWpiStr = wpi::util::make_string(line);
  return HAL_SendConsoleLine(&lineWpiStr);
}

inline int32_t SendProgramCrash(std::string_view details,
                                std::string_view location,
                                std::string_view callStack) {
  WPI_String detailsWpiStr = wpi::util::make_string(details);
  WPI_String locationWpiStr = wpi::util::make_string(location);
  WPI_String callStackWpiStr = wpi::util::make_string(callStack);
  return HAL_SendProgramCrash(&detailsWpiStr, &locationWpiStr,
                              &callStackWpiStr);
}

inline ControlWord GetControlWord() {
  HAL_ControlWord word;
  HAL_GetControlWord(&word);
  return ControlWord{word};
}

inline ControlWord GetUncachedControlWord() {
  HAL_ControlWord word;
  HAL_GetUncachedControlWord(&word);
  return ControlWord{word};
}
}  // namespace wpi::hal
