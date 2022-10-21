// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "LoggerImpl.h"

#include <fmt/format.h>
#include <wpi/DenseMap.h>
#include <wpi/fs.h>

using namespace nt;

static void DefaultLogger(unsigned int level, const char* file,
                          unsigned int line, const char* msg) {
  if (level == 20) {
    fmt::print(stderr, "NT: {}\n", msg);
    return;
  }

  std::string_view levelmsg;
  if (level >= 50) {
    levelmsg = "CRITICAL";
  } else if (level >= 40) {
    levelmsg = "ERROR";
  } else if (level >= 30) {
    levelmsg = "WARNING";
  } else {
    return;
  }
  fmt::print(stderr, "NT: {}: {} ({}:{})\n", levelmsg, msg, file, line);
}

class LoggerImpl::Thread final : public wpi::SafeThreadEvent {
 public:
  explicit Thread(NT_LoggerPoller poller) : m_poller{poller} {}

  void Main() final;

  NT_LoggerPoller m_poller;
  wpi::DenseMap<NT_Logger, std::function<void(const LogMessage& msg)>>
      m_callbacks;
};

void LoggerImpl::Thread::Main() {
  while (m_active) {
    WPI_Handle signaledBuf[2];
    auto signaled =
        wpi::WaitForObjects({m_poller, m_stopEvent.GetHandle()}, signaledBuf);
    if (signaled.empty() || !m_active) {
      return;
    }
    // call all the way back out to the C++ API to ensure valid handle
    auto events = nt::ReadLoggerQueue(m_poller);
    if (events.empty()) {
      continue;
    }
    std::unique_lock lock{m_mutex};
    for (auto&& event : events) {
      auto callbackIt = m_callbacks.find(event.logger);
      if (callbackIt != m_callbacks.end()) {
        auto callback = callbackIt->second;
        lock.unlock();
        callback(event);
        lock.lock();
      }
    }
  }
}

LoggerImpl::LoggerImpl(int inst) : m_inst{inst} {}

LoggerImpl::~LoggerImpl() = default;

NT_Logger LoggerImpl::Add(std::function<void(const LogMessage& msg)> callback,
                          unsigned int minLevel, unsigned int maxLevel) {
  if (!m_thread) {
    m_thread.Start(CreatePoller());
  }
  if (auto thr = m_thread.GetThread()) {
    auto listener = AddPolled(thr->m_poller, minLevel, maxLevel);
    if (listener) {
      thr->m_callbacks.try_emplace(listener, std::move(callback));
    }
    return listener;
  } else {
    return {};
  }
}

NT_LoggerPoller LoggerImpl::CreatePoller() {
  std::scoped_lock lock{m_mutex};
  return m_pollers.Add(m_inst)->handle;
}

void LoggerImpl::DestroyPoller(NT_LoggerPoller pollerHandle) {
  std::scoped_lock lock{m_mutex};
  m_pollers.Remove(pollerHandle);
}

NT_Logger LoggerImpl::AddPolled(NT_LoggerPoller pollerHandle,
                                unsigned int minLevel, unsigned int maxLevel) {
  std::scoped_lock lock{m_mutex};
  if (auto poller = m_pollers.Get(pollerHandle)) {
    return m_listeners.Add(m_inst, poller, minLevel, maxLevel)->handle;
  } else {
    return {};
  }
}

std::vector<LogMessage> LoggerImpl::ReadQueue(NT_LoggerPoller pollerHandle) {
  std::scoped_lock lock{m_mutex};
  if (auto poller = m_pollers.Get(pollerHandle)) {
    std::vector<LogMessage> rv;
    rv.swap(poller->queue);
    return rv;
  } else {
    return {};
  }
}

void LoggerImpl::Remove(NT_Logger listenerHandle) {
  std::scoped_lock lock{m_mutex};
  m_listeners.Remove(listenerHandle);
  if (auto thr = m_thread.GetThread()) {
    thr->m_callbacks.erase(listenerHandle);
  }
}

unsigned int LoggerImpl::GetMinLevel() {
  // return 0;
  unsigned int level = NT_LOG_INFO;
  for (auto&& listener : m_listeners) {
    if (listener && listener->minLevel < level) {
      level = listener->minLevel;
    }
  }
  return level;
}

void LoggerImpl::Log(unsigned int level, const char* file, unsigned int line,
                     const char* msg) {
  auto filename = fs::path{file}.filename();
  {
    std::scoped_lock lock{m_mutex};
    if (m_listeners.empty()) {
      DefaultLogger(level, filename.string().c_str(), line, msg);
    } else {
      for (auto&& listener : m_listeners) {
        if (level >= listener->minLevel && level <= listener->maxLevel) {
          listener->poller->queue.emplace_back(listener->handle.GetHandle(),
                                               level, file, line, msg);
          listener->poller->handle.Set();
          listener->handle.Set();
        }
      }
    }
  }
}
