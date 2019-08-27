/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>
#include <vector>

#include <wpi/StringRef.h>

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
  explicit CSVLogFile(wpi::StringRef filePrefix = "log");

  template <typename Value, typename... Values>
  void Log(Value value, Values... values) {
    m_logFile.Log(std::to_string(value) + ", ");
    Log(values);
  }

 private:
  template <typename Value>
  void Log(Value value) {
    m_logFile.Log(std::to_string(value) + "\n");
  }

  LogFile m_logFile;
};

}  // namespace frc
