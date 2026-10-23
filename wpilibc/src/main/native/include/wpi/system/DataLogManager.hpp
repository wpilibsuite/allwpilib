// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>
#include <string_view>

namespace wpi::log {
class DataLog;
}  // namespace wpi::log

namespace wpi::util {
class Alert;
}  // namespace wpi::util

namespace wpi {

/**
 * Centralized data log that provides automatic data log file management. It
 * automatically cleans up old files when disk space is low and renames the file
 * based either on current date/time or (if available) competition match number.
 * The data file will be saved to a USB flash drive in a folder named "logs" if
 * one is attached, or to /home/systemcore/logs otherwise.
 *
 * Log files are initially named "WPILIB_TBD_{random}.wpilog" until the DS
 * connects. After the DS connects, the log file is renamed to
 * "WPILIB_yyyyMMdd_HHmmss.wpilog" (where the date/time is UTC). If the FMS is
 * connected and provides a match number, the log file is renamed to
 * "WPILIB_yyyyMMdd_HHmmss_{event}_{match}.wpilog".
 *
 * On startup, all existing WPILIB_TBD log files are deleted. If there is less
 * than 200 MB of free space on the target storage, non-match WPILIB_ log files
 * are deleted (oldest to newest) until there is 200 MB free OR there are 10
 * files remaining. Competition match logs are not deleted. An alert is active
 * while less than 400 MB is available, checked every 5 seconds. An
 * informational alert indicates when automatic directory selection falls back
 * to SystemCore internal storage.
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
   * Stop data log manager.
   */
  static void Stop();

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

  /**
   * Enable or disable logging of the console output. Defaults to enabled.
   * @param enabled  true to enable, false to disable
   */
  static void LogConsoleOutput(bool enabled);
};

namespace internal {

/**
 * Returns whether a data log filename has a competition match suffix.
 * For testing purposes only.
 */
bool IsDataLogMatchLog(std::string_view filename);

/**
 * Updates the data log storage alert from the available disk space. For
 * testing purposes only.
 *
 * @param alert Low disk space alert.
 * @param freeSpace Available disk space, in bytes.
 */
void UpdateDataLogFreeSpaceAlert(wpi::util::Alert& alert, uintmax_t freeSpace);

}  // namespace internal

}  // namespace wpi
