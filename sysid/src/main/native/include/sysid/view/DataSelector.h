// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <glass/View.h>

namespace glass {
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
      : m_logger(logger) {}

  /**
   * Displays the log loader window.
   */
  void Display() override;

 private:
  wpi::Logger& m_logger;
};
}  // namespace sysid
