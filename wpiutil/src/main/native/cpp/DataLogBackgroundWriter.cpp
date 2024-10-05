// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/DataLogBackgroundWriter.h"

#ifndef _WIN32
#include <unistd.h>
#endif

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>  // NOLINT(build/include_order)

#endif

#include <random>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include "wpi/Logger.h"
#include "wpi/fs.h"

using namespace wpi::log;

static constexpr uintmax_t kMinFreeSpace = 5 * 1024 * 1024;

static std::string FormatBytesSize(uintmax_t value) {
  static constexpr uintmax_t kKiB = 1024;
  static constexpr uintmax_t kMiB = kKiB * 1024;
  static constexpr uintmax_t kGiB = kMiB * 1024;
  if (value >= kGiB) {
    return fmt::format("{:.1f} GiB", static_cast<double>(value) / kGiB);
  } else if (value >= kMiB) {
    return fmt::format("{:.1f} MiB", static_cast<double>(value) / kMiB);
  } else if (value >= kKiB) {
    return fmt::format("{:.1f} KiB", static_cast<double>(value) / kKiB);
  } else {
    return fmt::format("{} B", value);
  }
}

DataLogBackgroundWriter::DataLogBackgroundWriter(std::string_view dir,
                                                 std::string_view filename,
                                                 double period,
                                                 std::string_view extraHeader)
    : DataLogBackgroundWriter{s_defaultMessageLog, dir, filename, period,
                              extraHeader} {}

DataLogBackgroundWriter::DataLogBackgroundWriter(wpi::Logger& msglog,
                                                 std::string_view dir,
                                                 std::string_view filename,
                                                 double period,
                                                 std::string_view extraHeader)
    : DataLog{msglog, extraHeader},
      m_period{period},
      m_newFilename{filename},
      m_thread{[this, dir = std::string{dir}] { WriterThreadMain(dir); }} {}

DataLogBackgroundWriter::DataLogBackgroundWriter(
    std::function<void(std::span<const uint8_t> data)> write, double period,
    std::string_view extraHeader)
    : DataLogBackgroundWriter{s_defaultMessageLog, std::move(write), period,
                              extraHeader} {}

DataLogBackgroundWriter::DataLogBackgroundWriter(
    wpi::Logger& msglog,
    std::function<void(std::span<const uint8_t> data)> write, double period,
    std::string_view extraHeader)
    : DataLog{msglog, extraHeader},
      m_period{period},
      m_thread{[this, write = std::move(write)] {
        WriterThreadMain(std::move(write));
      }} {}

DataLogBackgroundWriter::~DataLogBackgroundWriter() {
  {
    std::scoped_lock lock{m_mutex};
    m_shutdown = true;
    m_doFlush = true;
  }
  m_cond.notify_all();
  m_thread.join();
}

void DataLogBackgroundWriter::SetFilename(std::string_view filename) {
  {
    std::scoped_lock lock{m_mutex};
    m_newFilename = filename;
  }
  m_cond.notify_all();
}

void DataLogBackgroundWriter::Flush() {
  {
    std::scoped_lock lock{m_mutex};
    m_doFlush = true;
  }
  m_cond.notify_all();
}

void DataLogBackgroundWriter::Pause() {
  DataLog::Pause();
  std::scoped_lock lock{m_mutex};
  m_state = kPaused;
}

void DataLogBackgroundWriter::Resume() {
  DataLog::Resume();
  std::scoped_lock lock{m_mutex};
  if (m_state == kPaused) {
    m_state = kActive;
  } else if (m_state == kStopped) {
    m_state = kStart;
  }
}

void DataLogBackgroundWriter::Stop() {
  DataLog::Stop();
  {
    std::scoped_lock lock{m_mutex};
    m_state = kStopped;
    m_newFilename.clear();
  }
  m_cond.notify_all();
}

static void WriteToFile(fs::file_t f, std::span<const uint8_t> data,
                        std::string_view filename, wpi::Logger& msglog) {
  do {
#ifdef _WIN32
    DWORD ret;
    if (!WriteFile(f, data.data(), data.size(), &ret, nullptr)) {
      WPI_ERROR(msglog, "Error writing to log file '{}': {}", filename,
                GetLastError());
      break;
    }
#else
    ssize_t ret = ::write(f, data.data(), data.size());
    if (ret < 0) {
      // If it's a recoverable error, swallow it and retry the write
      if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
        continue;
      }

      // Otherwise it's a non-recoverable error; quit trying
      WPI_ERROR(msglog, "Error writing to log file '{}': {}", filename,
                std::strerror(errno));
      break;
    }
#endif

    // The write may have written some or all of the data
    data = data.subspan(ret);
  } while (data.size() > 0);
}

static std::string MakeRandomFilename() {
  // build random filename
  static std::random_device dev;
  static std::mt19937 rng(dev());
  std::uniform_int_distribution<int> dist(0, 15);
  const char* v = "0123456789abcdef";
  std::string filename = "wpilog_";
  for (int i = 0; i < 16; i++) {
    filename += v[dist(rng)];
  }
  filename += ".wpilog";
  return filename;
}

struct DataLogBackgroundWriter::WriterThreadState {
  explicit WriterThreadState(std::string_view dir)
      : dirPath{dir.empty() ? "." : dir} {}
  WriterThreadState(const WriterThreadState&) = delete;
  WriterThreadState& operator=(const WriterThreadState&) = delete;
  ~WriterThreadState() { Close(); }

  void Close() {
    if (f != fs::kInvalidFile) {
      fs::CloseFile(f);
      f = fs::kInvalidFile;
    }
  }

  void SetFilename(std::string_view fn) {
    baseFilename = fn;
    filename = fn;
    path = dirPath / filename;
    segmentCount = 1;
  }

  void IncrementFilename() {
    fs::path basePath{baseFilename};
    filename = fmt::format("{}.{}{}", basePath.stem().string(), ++segmentCount,
                           basePath.extension().string());
    path = dirPath / filename;
  }

  fs::path dirPath;
  std::string baseFilename;
  std::string filename;
  fs::path path;
  fs::file_t f = fs::kInvalidFile;
  uintmax_t freeSpace = UINTMAX_MAX;
  int segmentCount = 1;
};

void DataLogBackgroundWriter::BufferHalfFull() {
  Flush();
}

bool DataLogBackgroundWriter::BufferFull() {
  WPI_ERROR(m_msglog,
            "outgoing buffers exceeded threshold, pausing logging--"
            "consider flushing to disk more frequently (smaller period)");
  return true;
}

void DataLogBackgroundWriter::StartLogFile(WriterThreadState& state) {
  std::error_code ec;

  if (state.filename.empty()) {
    state.SetFilename(MakeRandomFilename());
  }

  // get free space
  auto freeSpaceInfo = fs::space(state.dirPath, ec);
  if (!ec) {
    state.freeSpace = freeSpaceInfo.available;
  } else {
    state.freeSpace = UINTMAX_MAX;
  }
  if (state.freeSpace < kMinFreeSpace) {
    WPI_ERROR(m_msglog,
              "Insufficient free space ({} available), no log being saved",
              FormatBytesSize(state.freeSpace));
    m_state = kStopped;
  } else {
    // try preferred filename, or randomize it a few times, before giving up
    for (int i = 0; i < 5; ++i) {
      // open file for append
#ifdef _WIN32
      // WIN32 doesn't allow combination of CreateNew and Append
      state.f =
          fs::OpenFileForWrite(state.path, ec, fs::CD_CreateNew, fs::OF_None);
#else
      state.f =
          fs::OpenFileForWrite(state.path, ec, fs::CD_CreateNew, fs::OF_Append);
#endif
      if (ec) {
        WPI_ERROR(m_msglog, "Could not open log file '{}': {}",
                  state.path.string(), ec.message());
        // try again with random filename
        state.SetFilename(MakeRandomFilename());
      } else {
        break;
      }
    }

    if (state.f == fs::kInvalidFile) {
      WPI_ERROR(m_msglog, "Could not open log file, no log being saved");
    } else {
      WPI_INFO(m_msglog, "Logging to '{}' ({} free space)", state.path.string(),
               FormatBytesSize(state.freeSpace));
    }
  }

  // start file
  if (state.f != fs::kInvalidFile) {
    StartFile();
  }
}

void DataLogBackgroundWriter::WriterThreadMain(std::string_view dir) {
  std::chrono::duration<double> periodTime{m_period};

  WriterThreadState state{dir};
  {
    std::scoped_lock lock{m_mutex};
    state.SetFilename(m_newFilename);
    m_newFilename.clear();
  }
  StartLogFile(state);

  std::error_code ec;
  std::vector<DataLog::Buffer> toWrite;
  int freeSpaceCount = 0;
  int checkExistCount = 0;
  bool blocked = false;
  uintmax_t written = 0;

  std::unique_lock lock{m_mutex};
  do {
    bool doFlush = false;
    auto timeoutTime = std::chrono::steady_clock::now() + periodTime;
    if (m_cond.wait_until(lock, timeoutTime) == std::cv_status::timeout) {
      doFlush = true;
    }

    if (m_state == kStopped) {
      state.Close();
      continue;
    }

    bool doStart = false;

    // if file was deleted, recreate it with the same name
    if (++checkExistCount >= 10) {
      checkExistCount = 0;
      lock.unlock();
      bool exists = fs::exists(state.path, ec);
      lock.lock();
      if (!ec && !exists) {
        state.Close();
        state.IncrementFilename();
        WPI_INFO(m_msglog, "Log file deleted, recreating as fresh log '{}'",
                 state.filename);
        doStart = true;
      }
    }

    // start new file if file exceeds 1.8 GB
    if (written > 1800000000ull) {
      state.Close();
      state.IncrementFilename();
      WPI_INFO(m_msglog, "Log file reached 1.8 GB, starting new file '{}'",
               state.filename);
      doStart = true;
    }

    if (m_state == kStart || doStart) {
      lock.unlock();
      DataLog::Stop();
      StartLogFile(state);
      lock.lock();
      if (m_state == kStopped) {
        continue;
      }
      m_state = kActive;
      written = 0;
    }

    if (!m_newFilename.empty() && state.f != fs::kInvalidFile) {
      auto newFilename = std::move(m_newFilename);
      m_newFilename.clear();
      // rename
      if (state.filename != newFilename) {
        lock.unlock();
        fs::rename(state.path, state.dirPath / newFilename, ec);
        lock.lock();
      }
      if (ec) {
        WPI_ERROR(m_msglog, "Could not rename log file from '{}' to '{}': {}",
                  state.filename, newFilename, ec.message());
      } else {
        WPI_INFO(m_msglog, "Renamed log file from '{}' to '{}'", state.filename,
                 newFilename);
      }
      state.SetFilename(newFilename);
    }

    if (doFlush || m_doFlush) {
      // flush to file
      m_doFlush = false;
      DataLog::FlushBufs(&toWrite);
      if (toWrite.empty()) {
        continue;
      }

      if (state.f != fs::kInvalidFile && !blocked) {
        lock.unlock();

        // update free space every 10 flushes (in case other things are writing)
        if (++freeSpaceCount >= 10) {
          freeSpaceCount = 0;
          auto freeSpaceInfo = fs::space(state.dirPath, ec);
          if (!ec) {
            state.freeSpace = freeSpaceInfo.available;
          } else {
            state.freeSpace = UINTMAX_MAX;
          }
        }

        // write buffers to file
        for (auto&& buf : toWrite) {
          // stop writing when we go below the minimum free space
          state.freeSpace -= buf.GetData().size();
          written += buf.GetData().size();
          if (state.freeSpace < kMinFreeSpace) {
            [[unlikely]] WPI_ERROR(
                m_msglog,
                "Stopped logging due to low free space ({} available)",
                FormatBytesSize(state.freeSpace));
            blocked = true;
            break;
          }
          WriteToFile(state.f, buf.GetData(), state.filename, m_msglog);
        }

        // sync to storage
#if defined(__linux__)
        ::fdatasync(state.f);
#elif defined(__APPLE__)
        ::fsync(state.f);
#endif
        lock.lock();
        if (blocked) {
          [[unlikely]] m_state = kPaused;
        }
      }

      // release buffers back to free list
      ReleaseBufs(&toWrite);
    }
  } while (!m_shutdown);
}

void DataLogBackgroundWriter::WriterThreadMain(
    std::function<void(std::span<const uint8_t> data)> write) {
  std::chrono::duration<double> periodTime{m_period};

  StartFile();

  std::vector<DataLog::Buffer> toWrite;

  std::unique_lock lock{m_mutex};
  do {
    bool doFlush = false;
    auto timeoutTime = std::chrono::steady_clock::now() + periodTime;
    if (m_cond.wait_until(lock, timeoutTime) == std::cv_status::timeout) {
      doFlush = true;
    }

    if (doFlush || m_doFlush) {
      // flush to file
      m_doFlush = false;
      DataLog::FlushBufs(&toWrite);
      if (toWrite.empty()) {
        continue;
      }

      lock.unlock();
      // write buffers
      for (auto&& buf : toWrite) {
        if (!buf.GetData().empty()) {
          write(buf.GetData());
        }
      }
      lock.lock();

      // release buffers back to free list
      ReleaseBufs(&toWrite);
    }
  } while (!m_shutdown);

  write({});  // indicate EOF
}

extern "C" {

struct WPI_DataLog* WPI_DataLog_CreateBackgroundWriter(
    const struct WPI_String* dir, const struct WPI_String* filename,
    double period, const struct WPI_String* extraHeader) {
  return reinterpret_cast<WPI_DataLog*>(new DataLogBackgroundWriter{
      wpi::to_string_view(dir), wpi::to_string_view(filename), period,
      wpi::to_string_view(extraHeader)});
}

struct WPI_DataLog* WPI_DataLog_CreateBackgroundWriter_Func(
    void (*write)(void* ptr, const uint8_t* data, size_t len), void* ptr,
    double period, const struct WPI_String* extraHeader) {
  return reinterpret_cast<WPI_DataLog*>(new DataLogBackgroundWriter{
      [=](auto data) { write(ptr, data.data(), data.size()); }, period,
      wpi::to_string_view(extraHeader)});
}

void WPI_DataLog_SetBackgroundWriterFilename(
    struct WPI_DataLog* datalog, const struct WPI_String* filename) {
  reinterpret_cast<DataLogBackgroundWriter*>(datalog)->SetFilename(
      wpi::to_string_view(filename));
}

}  // extern "C"
