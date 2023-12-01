// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>

#include <glass/DataSource.h>
#include <glass/View.h>
#include <glass/networktables/NetworkTablesSettings.h>
#include <portable-file-dialogs.h>
#include <wpi/Logger.h>

#include "sysid/telemetry/TelemetryManager.h"

namespace glass {
class Storage;
}  // namespace glass

namespace sysid {
/**
 * The logger GUI takes care of running the system idenfitication tests over
 * NetworkTables and logging the data. This data is then stored in a JSON file
 * which can be used for analysis.
 */
class Logger : public glass::View {
 public:
  /**
   * Makes a logger widget.
   *
   * @param storage The glass storage object
   * @param logger A logger object that keeps track of the program's logs
   */
  Logger(glass::Storage& storage, wpi::Logger& logger);

  /**
   * Displays the logger widget.
   */
  void Display() override;

  /**
   * The different mechanism / analysis types that are supported.
   */
  static constexpr const char* kTypes[] = {"Drivetrain", "Drivetrain (Angular)",
                                           "Arm", "Elevator", "Simple"};

  /**
   * The different units that are supported.
   */
  static constexpr const char* kUnits[] = {"Meters",  "Feet",      "Inches",
                                           "Radians", "Rotations", "Degrees"};

 private:
  /**
   * Handles the logic of selecting a folder to save the SysId JSON to
   */
  void SelectDataFolder();

  wpi::Logger& m_logger;

  TelemetryManager::Settings m_settings;
  int m_selectedType = 0;
  int m_selectedUnit = 0;

  std::unique_ptr<TelemetryManager> m_manager =
      std::make_unique<TelemetryManager>(m_settings, m_logger);

  std::unique_ptr<pfd::select_folder> m_selector;
  std::string m_jsonLocation;

  glass::NetworkTablesSettings m_ntSettings;

  bool m_isRotationalUnits = false;

  std::string m_popupText;

  std::string m_opened;
  std::string m_exception;
};
}  // namespace sysid
