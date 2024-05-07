// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <thread>

#include "wpi/DataLog.h"
#include "wpi/condition_variable.h"
#include "wpi/mutex.h"

namespace wpi {
class Logger;
}  // namespace wpi

namespace wpi::log {

/**
 * A data log background writer that periodically flushes the data log on a
 * background thread.  The data log file is created immediately upon
 * construction with a temporary filename.  The file may be renamed at any time
 * using the SetFilename() function.
 *
 * The lifetime of this object must be longer than any data log entry objects
 * that refer to it.
 *
 * The data log is periodically flushed to disk.  It can also be explicitly
 * flushed to disk by using the Flush() function.  This operation is, however,
 * non-blocking.
 */
class DataLogBackgroundWriter final : public DataLog {
 public:
  /**
   * Construct a new Data Log.  The log will be initially created with a
   * temporary filename.
   *
   * @param dir directory to store the log
   * @param filename filename to use; if none provided, a random filename is
   *                 generated of the form "wpilog_{}.wpilog"
   * @param period time between automatic flushes to disk, in seconds;
   *               this is a time/storage tradeoff
   * @param extraHeader extra header data
   */
  explicit DataLogBackgroundWriter(std::string_view dir = "",
                                   std::string_view filename = "",
                                   double period = 0.25,
                                   std::string_view extraHeader = "");

  /**
   * Construct a new Data Log.  The log will be initially created with a
   * temporary filename.
   *
   * @param msglog message logger (will be called from separate thread)
   * @param dir directory to store the log
   * @param filename filename to use; if none provided, a random filename is
   *                 generated of the form "wpilog_{}.wpilog"
   * @param period time between automatic flushes to disk, in seconds;
   *               this is a time/storage tradeoff
   * @param extraHeader extra header data
   */
  explicit DataLogBackgroundWriter(wpi::Logger& msglog,
                                   std::string_view dir = "",
                                   std::string_view filename = "",
                                   double period = 0.25,
                                   std::string_view extraHeader = "");

  /**
   * Construct a new Data Log that passes its output to the provided function
   * rather than a file.  The write function will be called on a separate
   * background thread and may block.  The write function is called with an
   * empty data array when the thread is terminating.
   *
   * @param write write function
   * @param period time between automatic calls to write, in seconds;
   *               this is a time/storage tradeoff
   * @param extraHeader extra header data
   */
  explicit DataLogBackgroundWriter(
      std::function<void(std::span<const uint8_t> data)> write,
      double period = 0.25, std::string_view extraHeader = "");

  /**
   * Construct a new Data Log that passes its output to the provided function
   * rather than a file.  The write function will be called on a separate
   * background thread and may block.  The write function is called with an
   * empty data array when the thread is terminating.
   *
   * @param msglog message logger (will be called from separate thread)
   * @param write write function
   * @param period time between automatic calls to write, in seconds;
   *               this is a time/storage tradeoff
   * @param extraHeader extra header data
   */
  explicit DataLogBackgroundWriter(
      wpi::Logger& msglog,
      std::function<void(std::span<const uint8_t> data)> write,
      double period = 0.25, std::string_view extraHeader = "");

  ~DataLogBackgroundWriter() final;
  DataLogBackgroundWriter(const DataLogBackgroundWriter&) = delete;
  DataLogBackgroundWriter& operator=(const DataLogBackgroundWriter&) = delete;
  DataLogBackgroundWriter(DataLogBackgroundWriter&&) = delete;
  DataLogBackgroundWriter& operator=(const DataLogBackgroundWriter&&) = delete;

  /**
   * Change log filename.
   *
   * @param filename filename
   */
  void SetFilename(std::string_view filename);

  /**
   * Explicitly flushes the log data to disk.
   */
  void Flush() final;

  /**
   * Pauses appending of data records to the log.  While paused, no data records
   * are saved (e.g. AppendX is a no-op).  Has no effect on entry starts /
   * finishes / metadata changes.
   */
  void Pause() final;

  /**
   * Resumes appending of data records to the log.  If called after Stop(),
   * opens a new file (with random name if SetFilename was not called after
   * Stop()) and appends Start records and schema data values for all previously
   * started entries and schemas.
   */
  void Resume() final;

  /**
   * Stops appending all records to the log, and closes the log file.
   */
  void Stop() final;

 private:
  struct WriterThreadState;

  void BufferHalfFull() final;
  bool BufferFull() final;

  void StartLogFile(WriterThreadState& state);
  void WriterThreadMain(std::string_view dir);
  void WriterThreadMain(
      std::function<void(std::span<const uint8_t> data)> write);

  mutable wpi::mutex m_mutex;
  wpi::condition_variable m_cond;
  bool m_doFlush{false};
  bool m_shutdown{false};
  enum State {
    kStart,
    kActive,
    kPaused,
    kStopped,
  } m_state = kActive;
  double m_period;
  std::string m_newFilename;
  std::thread m_thread;
};

}  // namespace wpi::log
