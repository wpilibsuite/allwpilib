/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <chrono>
#include <string>
#include <type_traits>

#include <wpi/StringRef.h>
#include <wpi/Twine.h>

#include "frc/logging/LogFile.h"

namespace frc {

/**
 * A CSVLogFile writes to a csv file
 *
 * For the CSVLogFile to write log informations, you must call Log()
 * periodically.
 */
class CSVLogFile {
 public:
  /**
   * Instantiate a LogFile passing in its prefix and its extension.
   *
   * If you want the file to be saved in a existing directory, you can add its
   * path before the file prefix. Exemple : to save the file in a usb stick on
   * the roborio ("/media/sda1/") : LogFile("/media/sda1/log").
   *
   * @param filePrefix The prefix of the LogFile.
   */
  template <typename Value, typename... Values>
  CSVLogFile(wpi::StringRef filePrefix, Value columnHeading,
             Values... columnHeadings)
      : m_logFile(filePrefix, "csv") {
    m_logFile << "Timestamp (ms),";
    LogValues(columnHeading, columnHeadings...);
  }

  template <typename Value, typename... Values>
  void Log(Value value, Values... values) {
    using namespace std::chrono;
    auto timestamp =
        duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    m_logFile << timestamp.count() << ",";

    LogValues(value, values...);
  }

 private:
  template <typename Value, typename... Values>
  void LogValues(Value value, Values... values) {
    if constexpr (std::is_convertible_v<Value, wpi::StringRef>) {
      m_logFile << "\"" << value << "\"";
    } else {
      m_logFile << value;
    }
    if constexpr (sizeof...(values) > 0) {
      m_logFile << ",";
      LogValues(values...);
    } else {
      m_logFile << "\n";
    }
  }

  LogFile m_logFile;
};

}  // namespace frc
