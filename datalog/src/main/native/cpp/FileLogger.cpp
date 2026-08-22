// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/datalog/FileLogger.hpp"

#ifdef __linux__
#include <fcntl.h>
#include <poll.h>
#include <sys/inotify.h>
#include <unistd.h>
#endif

#include <chrono>
#include <format>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <utility>

#include "wpi/util/StringExtras.hpp"

namespace wpi::log {
FileLogger::FileLogger(std::string_view file,
                       std::function<void(std::string_view)> callback) {
#ifdef __linux__
  std::string fileStr{file};
  m_fileHandle = open(fileStr.c_str(), O_RDONLY);
  m_inotifyHandle = inotify_init();
  m_inotifyWatchHandle =
      inotify_add_watch(m_inotifyHandle, fileStr.c_str(), IN_MODIFY);

  // inotify watches the file's inode, so a file that does not exist yet will
  // never generate events (and a file created later won't be picked up).
  // Don't spawn a thread in that case; without a watch, a reader blocked in
  // read() can never be woken up and the destructor would deadlock.
  if (m_fileHandle == -1 || m_inotifyWatchHandle == -1) {
    return;
  }
  m_thread =
      std::thread{[callback = std::move(callback), fileHandle = m_fileHandle,
                   inotifyHandle = m_inotifyHandle, running = m_running] {
        char buf[8000];
        char eventBuf[sizeof(struct inotify_event) + NAME_MAX + 1];
        lseek(fileHandle, 0, SEEK_END);
        while (running->load()) {
          // poll() with a timeout instead of a blocking read() so the thread
          // periodically checks m_running and the destructor can join it even
          // when the watched file is gone and no events ever arrive.
          struct pollfd pfd{inotifyHandle, POLLIN, 0};
          if (poll(&pfd, 1, 100) <= 0) {
            continue;
          }
          if ((pfd.revents & POLLNVAL) != 0) {
            break;
          }
          if ((pfd.revents & POLLIN) != 0 &&
              read(inotifyHandle, eventBuf, sizeof(eventBuf)) > 0) {
            int bufLen = 0;
            if ((bufLen = read(fileHandle, buf, sizeof(buf))) > 0) {
              callback(std::string_view{buf, static_cast<size_t>(bufLen)});
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
          }
        }
      }};
#endif
}
FileLogger::FileLogger(std::string_view file, log::DataLog& log,
                       std::string_view key)
    : FileLogger(file, Buffer(log.MakeFileLoggerCallback(key))) {}
FileLogger::FileLogger(FileLogger&& other)
#ifdef __linux__
    : m_fileHandle{std::exchange(other.m_fileHandle, -1)},
      m_inotifyHandle{std::exchange(other.m_inotifyHandle, -1)},
      m_inotifyWatchHandle{std::exchange(other.m_inotifyWatchHandle, -1)},
      m_running{std::move(other.m_running)},
      m_thread{std::move(other.m_thread)}
#endif
{
}
FileLogger& FileLogger::operator=(FileLogger&& rhs) {
#ifdef __linux__
  if (this == &rhs) {
    return *this;
  }

  Stop();
  m_fileHandle = std::exchange(rhs.m_fileHandle, -1);
  m_inotifyHandle = std::exchange(rhs.m_inotifyHandle, -1);
  m_inotifyWatchHandle = std::exchange(rhs.m_inotifyWatchHandle, -1);
  m_running = std::move(rhs.m_running);
  m_thread = std::move(rhs.m_thread);
#endif
  return *this;
}
FileLogger::~FileLogger() {
#ifdef __linux__
  Stop();
#endif
}

#ifdef __linux__
void FileLogger::Stop() {
  if (m_running) {
    m_running->store(false);
  }
  if (m_thread.joinable()) {
    m_thread.join();
  }
  if (m_inotifyWatchHandle != -1) {
    inotify_rm_watch(m_inotifyHandle, m_inotifyWatchHandle);
    m_inotifyWatchHandle = -1;
  }
  if (m_inotifyHandle != -1) {
    close(m_inotifyHandle);
    m_inotifyHandle = -1;
  }
  if (m_fileHandle != -1) {
    close(m_fileHandle);
    m_fileHandle = -1;
  }
}
#endif

std::function<void(std::string_view)> FileLogger::Buffer(
    std::function<void(std::string_view)> callback) {
  return [callback, buf = wpi::util::SmallVector<char, 64>{}](
             std::string_view data) mutable {
    buf.append(data.begin(), data.end());
    if (!wpi::util::contains({data.data(), data.size()}, "\n")) {
      return;
    }
    auto [wholeData, extra] = wpi::util::rsplit({buf.data(), buf.size()}, "\n");
    std::string leftover{extra};

    callback(wholeData);
    buf.clear();
    buf.append(leftover.begin(), leftover.end());
  };
}
}  // namespace wpi::log
