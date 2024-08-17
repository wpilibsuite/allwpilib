// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/FileLogger.h"

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
FileLogger::FileLogger(FileLogger&& other)
#ifdef __linux__
    : m_fileHandle{std::move(other.m_fileHandle)},
      m_inotifyHandle{std::move(other.m_inotifyHandle)},
      m_inotifyWatchHandle{std::move(other.m_inotifyWatchHandle)},
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
