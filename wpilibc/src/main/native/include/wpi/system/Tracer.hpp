// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <chrono>
#include <string_view>

#include "wpi/hal/cpp/fpga_clock.h"
#include "wpi/util/StringMap.hpp"

namespace wpi::util {
class raw_ostream;
}  // namespace wpi::util

namespace wpi {
/**
 * A class for keeping track of how much time it takes for different parts of
 * code to execute. This is done with epochs, that are added to calls to
 * AddEpoch() and can be printed with a call to PrintEpochs().
 *
 * Epochs are a way to partition the time elapsed so that when overruns occur,
 * one can determine which parts of an operation consumed the most time.
 */
class Tracer {
 public:
  /**
   * Constructs a Tracer instance.
   */
  Tracer();

  /**
   * Restarts the epoch timer.
   */
  void ResetTimer();

  /**
   * Clears all epochs.
   */
  void ClearEpochs();

  /**
   * Adds time since last epoch to the list printed by PrintEpochs().
   *
   * Epochs are a way to partition the time elapsed so that when overruns occur,
   * one can determine which parts of an operation consumed the most time.
   *
   * @param epochName The name to associate with the epoch.
   */
  void AddEpoch(std::string_view epochName);

  /**
   * Prints list of epochs added so far and their times to the DriverStation.
   */
  void PrintEpochs();

  /**
   * Prints list of epochs added so far and their times to a stream.
   *
   * @param os output stream
   */
  void PrintEpochs(wpi::util::raw_ostream& os);

 private:
  static constexpr std::chrono::milliseconds kMinPrintPeriod{1000};

  wpi::hal::fpga_clock::time_point m_startTime;
  wpi::hal::fpga_clock::time_point m_lastEpochsPrintTime =
      wpi::hal::fpga_clock::epoch();

  wpi::util::StringMap<std::chrono::nanoseconds> m_epochs;
};
}  // namespace wpi
