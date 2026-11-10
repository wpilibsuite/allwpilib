// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/datalog/DataLog.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/system/DataLogManager.hpp"

/**
 * Snippets that logs custom data to the WPILib data log for wpilib-docs.
 * https://docs.wpilib.org/en/stable/docs/software/telemetry/datalog.html
 */
class Robot : public wpi::TimedRobot {
 public:
  /** Set up the custom entries. */
  Robot() {
    // Starts recording to data log.
    wpi::DataLogManager::Start();

    // Set up custom log entries.
    wpi::log::DataLog& log = wpi::DataLogManager::GetLog();
    m_booleanLog = wpi::log::BooleanLogEntry{log, "/my/boolean"};
    m_doubleLog = wpi::log::DoubleLogEntry{log, "/my/double"};
    m_stringLog = wpi::log::StringLogEntry{log, "/my/string"};
  }

  /** Log the custom entries periodically. */
  void TeleopPeriodic() override {
    m_booleanLog.Append(true);
    m_doubleLog.Append(3.5);
    m_stringLog.Append("example");
  }

 private:
  wpi::log::BooleanLogEntry m_booleanLog;
  wpi::log::DoubleLogEntry m_doubleLog;
  wpi::log::StringLogEntry m_stringLog;
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
