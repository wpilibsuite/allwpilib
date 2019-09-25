/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <algorithm>
#include <chrono>
#include <string>
#include <type_traits>

#include <wpi/StringRef.h>

#include "frc/logging/LogFile.h"

namespace frc {

/**
 * A CSVLogFile writes values to a csv file
 *
 * For the CSVLogFile to write log informations, you must call Log()
 * periodically.
 */
class CSVLogFile {
 public:
  /**
   * Instantiate a LogFile passing in its prefix and its column headings.
   *
   * If you want the file to be saved in a existing directory, you can add
   * its path before the file prefix. Exemple : to save the file in a usb stick
   * on the roborio ("/media/sda1/") : LogFile("/media/sda1/log").
   *
   * @param filePrefix     The prefix of the LogFile.
   * @param columnHeading  Title of 1st CSVLogFile column.
   * @param columnHeadings Titles of other CSVLogFile columns.
   */
  template <typename Value, typename... Values>
  CSVLogFile(wpi::StringRef filePrefix, Value columnHeading,
             Values... columnHeadings)
      : m_logFile(filePrefix, "csv") {
    m_logFile << "\"Timestamp (ms)\",";
    LogValues(columnHeading, columnHeadings...);
  }

  /**
   * Print a new line of values in the CSVLogFile.
   *
   * @param value 1st value to log in the file.
   * @param values Other values to log in the file in the order.
   */
  template <typename Value, typename... Values>
  void Log(Value value, Values... values) {
    using namespace std::chrono;
    auto timestamp =
        duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    m_logFile << timestamp.count() << ',';

    LogValues(value, values...);
  }

  /**
   * Get the name the file.
   *
   * @return The name of the file.
   */
  const std::string GetFileName() const { return m_logFile.GetFileName(); }

 private:
  /**
   * Print a new line of values in the CSVLogFile without timestamp.
   *
   * @param value 1st value to log in the file.
   * @param values Other values to log in the file in the order.
   */
  template <typename Value, typename... Values>
  void LogValues(Value value, Values... values) {
    if constexpr (std::is_convertible_v<Value, wpi::StringRef>) {
      m_logFile << '\"' << EscapeDoubleQuotes(value) << '\"';
    } else {
      m_logFile << value;
    }

    if constexpr (sizeof...(values) > 0) {
      m_logFile << ',';
      LogValues(values...);
    } else {
      m_logFile << '\n';
    }
  }

  /**
   * Escape double quotes in a text by duplicating them.
   *
   * @param text Text to escape.
   * @return The text with all its double quotes escaped.
   */
  const std::string EscapeDoubleQuotes(wpi::StringRef text) const {
    std::string textString = text.str();
    for (std::string::size_type i = 0; i < text.size(); i++) {
      if (text[i] == '\"') {
        i++;
        textString.insert(i, "\"");
      }
    }
    return textString;
  }

  LogFile m_logFile;
};

}  // namespace frc
