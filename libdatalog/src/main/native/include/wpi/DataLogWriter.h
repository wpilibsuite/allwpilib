// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>
#include <system_error>

#include "wpi/DataLog.h"

namespace wpi {
class raw_ostream;
class Logger;
}  // namespace wpi

namespace wpi::log {

/**
 * A data log writer that flushes the data log to a file when Flush() is called.
 *
 * The lifetime of this object must be longer than any data log entry objects
 * that refer to it.
 */
class DataLogWriter final : public DataLog {
 public:
  /**
   * Constructs with a filename.
   *
   * @param filename filename to use
   * @param ec error code if failed to open file (output)
   * @param extraHeader extra header data
   */
  explicit DataLogWriter(std::string_view filename, std::error_code& ec,
                         std::string_view extraHeader = "");

  /**
   * Construct with a filename.
   *
   * @param msglog message logger
   * @param filename filename to use
   * @param ec error code if failed to open file (output)
   * @param extraHeader extra header data
   */
  DataLogWriter(wpi::Logger& msglog, std::string_view filename,
                std::error_code& ec, std::string_view extraHeader = "");

  /**
   * Constructs with an output stream.
   *
   * @param os output stream
   * @param extraHeader extra header data
   */
  explicit DataLogWriter(std::unique_ptr<wpi::raw_ostream> os,
                         std::string_view extraHeader = "");

  /**
   * Constructs with an output stream.
   *
   * @param msglog message logger
   * @param os output stream
   * @param extraHeader extra header data
   */
  DataLogWriter(wpi::Logger& msglog, std::unique_ptr<wpi::raw_ostream> os,
                std::string_view extraHeader = "");

  ~DataLogWriter() final;
  DataLogWriter(const DataLogWriter&) = delete;
  DataLogWriter& operator=(const DataLogWriter&) = delete;
  DataLogWriter(DataLogWriter&&) = delete;
  DataLogWriter& operator=(const DataLogWriter&&) = delete;

  /**
   * Flushes the log data to disk.
   */
  void Flush() final;

  /**
   * Stops appending all records to the log, and closes the log file.
   */
  void Stop() final;

  /**
   * Gets the output stream.
   *
   * @return output stream
   */
  wpi::raw_ostream& GetStream() { return *m_os; }

 private:
  bool BufferFull() final;

  std::unique_ptr<wpi::raw_ostream> m_os;
};

}  // namespace wpi::log
