// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <string_view>
#include <thread>

#include "wpi/DataLog.h"

namespace wpi {
/**
 * A class version of `tail -f`, otherwise known as `tail -f` at home.  Watches
 * a file and puts the data somewhere else. Only works on Linux-based platforms.
 */
class FileLogger {
 public:
  FileLogger() = default;
  /**
   * Construct a FileLogger. When the specified file is modified, the callback
   * will be called with the appended changes.
   *
   * @param file The path to the file.
   * @param callback A callback that accepts the appended file data.
   */
  FileLogger(std::string_view file,
             std::function<void(std::string_view)> callback);

  /**
   * Construct a FileLogger. When the specified file is modified, appended data
   * will be appended to the specified data log.
   *
   * @param file The path to the file.
   * @param log A data log.
   * @param key The log key to append data to.
   */
  FileLogger(std::string_view file, log::DataLog& log, std::string_view key);
  FileLogger(FileLogger&& other);
  FileLogger& operator=(FileLogger&& rhs);
  ~FileLogger();
  /**
   * Creates a function that chunks incoming data into blocks of whole lines and
   * stores incomplete lines to add to the next block of data.
   *
   * @param callback A callback that accepts the blocks of whole lines.
   * @return The function.
   */
  static std::function<void(std::string_view)> Buffer(
      std::function<void(std::string_view)> callback);

 private:
#ifdef __linux__
  int m_fileHandle = -1;
  int m_inotifyHandle = -1;
  int m_inotifyWatchHandle = -1;
  std::thread m_thread;
#endif
};
}  // namespace wpi
