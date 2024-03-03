// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <chrono>
#include <memory>
#include <string_view>

#include <hal/cpp/fpga_clock.h>
#include <networktables/IntegerTopic.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/DataLog.h>
#include <wpi/StringMap.h>

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
   * Starts publishing added epochs to NetworkTables. Subsequent calls will do
   * nothing.
   *
   * @param topicName The NetworkTables topic to publish to
   */
  void PublishToNetworkTables(std::string_view topicName);

  /**
   * Starts logging added epochs to the data log. Subsequent calls will do
   * nothing.
   *
   * @param dataLog The data log to log epochs to
   * @param entry The name of the entry to log to
   */
  void StartDataLog(wpi::log::DataLog& dataLog, std::string_view entry);

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
  void PrintEpochs(wpi::raw_ostream& os);

 private:
  static constexpr std::chrono::milliseconds kMinPrintPeriod{1000};

  hal::fpga_clock::time_point m_startTime;
  hal::fpga_clock::time_point m_lastEpochsPrintTime = hal::fpga_clock::epoch();
  bool m_publishNT = false;
  nt::NetworkTableInstance m_inst;
  std::string_view m_ntTopic;
  wpi::StringMap<std::shared_ptr<nt::IntegerPublisher>> m_publisherCache;
  bool m_dataLogEnabled = false;
  wpi::log::DataLog* m_dataLog;
  std::string_view m_dataLogEntry;
  wpi::StringMap<int> m_entryCache;

  wpi::StringMap<std::chrono::nanoseconds> m_epochs;
};
}  // namespace frc
