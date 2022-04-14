// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "LoggerImpl.h"

#include <fmt/format.h>
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

LoggerImpl::LoggerImpl(int inst) : m_inst(inst) {}

void LoggerImpl::Start() {
  DoStart(m_inst);
}

unsigned int LoggerImpl::Add(
    std::function<void(const LogMessage& msg)> callback, unsigned int min_level,
    unsigned int max_level) {
  return DoAdd(callback, min_level, max_level);
}

unsigned int LoggerImpl::AddPolled(unsigned int poller_uid,
                                   unsigned int min_level,
                                   unsigned int max_level) {
  return DoAdd(poller_uid, min_level, max_level);
}

unsigned int LoggerImpl::GetMinLevel() {
  auto thr = GetThread();
  if (!thr) {
    return NT_LOG_INFO;
  }
  unsigned int level = NT_LOG_INFO;
  for (size_t i = 0; i < thr->m_listeners.size(); ++i) {
    const auto& listener = thr->m_listeners[i];
    if (listener && listener.min_level < level) {
      level = listener.min_level;
    }
  }
  return level;
}

void LoggerImpl::Log(unsigned int level, const char* file, unsigned int line,
                     const char* msg) {
  auto filename = fs::path{file}.filename();
  {
    auto thr = GetThread();
    if (!thr || thr->m_listeners.empty()) {
      DefaultLogger(level, filename.string().c_str(), line, msg);
    }
  }
  Send(UINT_MAX, 0, level, filename.string(), line, msg);
}
