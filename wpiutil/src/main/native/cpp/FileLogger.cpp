// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/FileLogger.h"

#ifdef __linux__
#include <fcntl.h>
#include <sys/inotify.h>
#include <unistd.h>
#endif

#include <string>
#include <string_view>
#include <tuple>
#include <utility>

#include <fmt/format.h>

#include "wpi/StringExtras.h"

namespace wpi {
FileLogger::FileLogger(std::string_view file,
                       std::function<void(std::string_view)> callback)
#ifdef __linux__
    : m_fileHandle{open(file.data(), O_RDONLY)},
      m_inotifyHandle{inotify_init()},
      m_inotifyWatchHandle{
          inotify_add_watch(m_inotifyHandle, file.data(), IN_MODIFY)},
      m_thread{[=, this] {
        char buf[4000];
        struct inotify_event ev;
        int len = 0;
        lseek(m_fileHandle, 0, SEEK_END);
        while ((len = read(m_inotifyHandle, &ev, sizeof(ev))) > 0) {
          int bufLen = 0;
          if ((bufLen = read(m_fileHandle, buf, sizeof(buf)) > 0)) {
            callback(std::string_view{buf, static_cast<size_t>(bufLen)});
          }
        }
      }}
#endif
{
}
FileLogger::FileLogger(std::string_view file, log::DataLog& log,
                       std::string_view key)
    : FileLogger(file, [entry = log.Start(key, "string"),
                        buf = wpi::SmallVector<char, 16>{},
                        &log](std::string_view data) mutable {
        if (!wpi::contains(data, "\n")) {
          buf.append(data.begin(), data.end());
          return;
        }
        std::string_view line;
        std::string combinedData = fmt::format("{}{}", buf.data(), data);
        buf.clear();
        do {
          std::tie(line, combinedData) = wpi::split(combinedData, "\n");
          log.AppendString(entry, line, 0);
        } while (wpi::contains(combinedData, "\n"));
        buf.append(combinedData.begin(), combinedData.end());
      }) {}
FileLogger::FileLogger(FileLogger&& other)
#ifdef __linux__
    : m_fileHandle{std::exchange(other.m_fileHandle, -1)},
      m_inotifyHandle{std::exchange(other.m_inotifyHandle, -1)},
      m_inotifyWatchHandle{std::exchange(other.m_inotifyWatchHandle, -1)},
      m_thread{std::move(other.m_thread)}
#endif
{
}
FileLogger& FileLogger::operator=(FileLogger&& rhs) {
#ifdef __linux__
  std::swap(m_fileHandle, rhs.m_fileHandle);
  std::swap(m_inotifyHandle, rhs.m_inotifyHandle);
  std::swap(m_inotifyWatchHandle, rhs.m_inotifyWatchHandle);
  m_thread = std::move(rhs.m_thread);
#endif
  return *this;
}
FileLogger::~FileLogger() {
#ifdef __linux__
  if (m_inotifyWatchHandle != -1) {
    inotify_rm_watch(m_inotifyHandle, m_inotifyWatchHandle);
  }
  if (m_inotifyHandle != -1) {
    close(m_inotifyHandle);
  }
  if (m_fileHandle != -1) {
    close(m_fileHandle);
  }
  if (m_thread.joinable()) {
    m_thread.join();
  }
#endif
}
}  // namespace wpi
