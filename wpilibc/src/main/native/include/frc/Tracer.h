/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <chrono>

#include <hal/cpp/fpga_clock.h>
#include <wpi/StringMap.h>
#include <wpi/StringRef.h>

namespace wpi {
class raw_ostream;
}  // namespace wpi

namespace frc {
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
  void AddEpoch(wpi::StringRef epochName);

  /**
   * Prints list of epochs added so far and their times to the DriverStation.
   */
  void PrintEpochs();

  /**
   * Prints list of epochs added so far and their times to a stream.
   *
   * @param os output stream
   */
  void PrintEpochs(wpi::raw_ostream& os);

 private:
  static constexpr std::chrono::milliseconds kMinPrintPeriod{1000};

  hal::fpga_clock::time_point m_startTime;
  hal::fpga_clock::time_point m_lastEpochsPrintTime = hal::fpga_clock::epoch();

  wpi::StringMap<std::chrono::nanoseconds> m_epochs;
};
}  // namespace frc
