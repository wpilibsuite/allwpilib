// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

namespace wpi::log {
class DataLog;
}  // namespace wpi::log

namespace frc {

/**
 * Centralized data log that provides automatic data log file management. It
 * automatically cleans up old files when disk space is low and renames the file
 * based either on current date/time or (if available) competition match number.
 * The deta file will be saved to a USB flash drive if one is attached, or to
 * /home/lvuser otherwise.
 *
 * Log files are initially named "FRC_TBD_{random}.wpilog" until the DS
 * connects. After the DS connects, the log file is renamed to
 * "FRC_yyyyMMdd_HHmmss.wpilog" (where the date/time is UTC). If the FMS is
 * connected and provides a match number, the log file is renamed to
 * "FRC_yyyyMMdd_HHmmss_{event}_{match}.wpilog".
 *
 * On startup, all existing FRC_TBD log files are deleted. If there is less than
 * 50 MB of free space on the target storage, FRC_ log files are deleted (oldest
 * to newest) until there is 50 MB free OR there are 10 files remaining.
 *
 * By default, all NetworkTables value changes are stored to the data log.
 */
class DataLogManager final {
 public:
  DataLogManager() = delete;

  /**
   * Start data log manager. The parameters have no effect if the data log
   * manager was already started (e.g. by calling another static function).
   *
   * @param dir if not empty, directory to use for data log storage
   * @param filename filename to use; if none provided, the filename is
   *     automatically generated
   * @param period time between automatic flushes to disk, in seconds;
   *               this is a time/storage tradeoff
   */
  static void Start(std::string_view dir = "", std::string_view filename = "",
                    double period = 0.25);

  /**
   * Log a message to the "messages" entry. The message is also printed to
   * standard output (followed by a newline).
   *
   * @param message message
   */
  static void Log(std::string_view message);

  /**
   * Get the managed data log (for custom logging). Starts the data log manager
   * if not already started.
   *
   * @return data log
   */
  static wpi::log::DataLog& GetLog();

  /**
   * Get the log directory.
   *
   * @return log directory
   */
  static std::string GetLogDir();

  /**
   * Enable or disable logging of NetworkTables data. Note that unlike the
   * network interface for NetworkTables, this will capture every value change.
   * Defaults to enabled.
   *
   * @param enabled true to enable, false to disable
   */
  static void LogNetworkTables(bool enabled);
};

}  // namespace frc
