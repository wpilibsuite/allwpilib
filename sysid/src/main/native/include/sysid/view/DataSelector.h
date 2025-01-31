// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <future>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <glass/View.h>
#include <glass/support/DataLogReaderThread.h>
#include <wpi/StringMap.h>

#include "sysid/analysis/Storage.h"

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
      : m_logger{logger} {}

  /**
   * Displays the log loader window.
   */
  void Display() override;

  /**
   * Resets view. Must be called whenever the DataLogReader goes away, as this
   * class keeps references to DataLogReaderEntry objects.
   */
  void Reset();

  /**
   * Called when new test data is loaded.
   */
  std::function<void(TestData)> testdata;
  std::vector<std::string> m_missingTests;

 private:
  wpi::Logger& m_logger;
  using Runs = std::vector<std::pair<int64_t, int64_t>>;
  using State = std::map<std::string, Runs, std::less<>>;   // full name
  using Tests = std::map<std::string, State, std::less<>>;  // e.g. "dynamic"
  std::future<Tests> m_testsFuture;
  Tests m_tests;
  std::string m_selectedTest;
  const glass::DataLogReaderEntry* m_testStateEntry = nullptr;
  const glass::DataLogReaderEntry* m_velocityEntry = nullptr;
  const glass::DataLogReaderEntry* m_positionEntry = nullptr;
  const glass::DataLogReaderEntry* m_voltageEntry = nullptr;
  double m_velocityScale = 1.0;
  double m_positionScale = 1.0;
  int m_selectedUnit = 0;
  int m_selectedAnalysis = 0;
  std::future<TestData> m_testdataFuture;
  std::vector<std::string> m_testdataStats;
  std::set<std::string> kValidTests = {"quasistatic-forward",
                                       "quasistatic-reverse", "dynamic-forward",
                                       "dynamic-reverse"};
  std::set<std::string> m_executedTests;
  bool m_testCountValidated = false;

  static Tests LoadTests(const glass::DataLogReaderEntry& testStateEntry);
  TestData BuildTestData();
};
}  // namespace sysid
