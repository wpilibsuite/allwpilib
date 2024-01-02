// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <future>
#include <map>
#include <string>
#include <vector>

#include <glass/View.h>
#include <glass/support/DataLogReaderThread.h>
#include <wpi/StringMap.h>

namespace glass {
class DataLogReaderEntry;
class Storage;
}  // namespace glass

namespace wpi {
class Logger;
}  // namespace wpi

namespace sysid {
/**
 * Helps with loading datalog files.
 */
class DataSelector : public glass::View {
 public:
  /**
   * Creates a data selector widget
   *
   * @param logger The program logger
   */
  explicit DataSelector(glass::Storage& storage, wpi::Logger& logger)
      /*: m_logger{logger}*/ {}

  /**
   * Displays the log loader window.
   */
  void Display() override;

  /**
   * Resets view. Must be called whenever the DataLogReader goes away, as this
   * class keeps references to DataLogReaderEntry objects.
   */
  void Reset();

 private:
  void LoadTests(const glass::DataLogReaderEntry& testStateEntry);

  // wpi::Logger& m_logger;
  using Runs = std::vector<glass::DataLogReaderRange>;
  using State = std::map<std::string, Runs, std::less<>>;   // full name
  using Tests = std::map<std::string, State, std::less<>>;  // e.g. "dynamic"
  std::future<Tests> m_testsFuture;
  std::string m_selectedTest;
  Tests m_tests;
  const glass::DataLogReaderEntry* m_testStateEntry = nullptr;
  const glass::DataLogReaderEntry* m_velocityEntry = nullptr;
  const glass::DataLogReaderEntry* m_positionEntry = nullptr;
  const glass::DataLogReaderEntry* m_voltageEntry = nullptr;
};
}  // namespace sysid
