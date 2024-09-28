// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "LoggerImpl.h"

#include <string>

#include <wpi/Logger.h>
#include <wpi/SmallVector.h>
#include <wpi/fs.h>
#include <wpi/print.h>

#include "IListenerStorage.h"

using namespace nt;

static void DefaultLogger(unsigned int level, const char* file,
                          unsigned int line, const char* msg) {
  if (level == wpi::WPI_LOG_INFO) {
    wpi::print(stderr, "NT: {}\n", msg);
    return;
  }

  std::string_view levelmsg;
  if (level >= wpi::WPI_LOG_CRITICAL) {
    levelmsg = "CRITICAL";
  } else if (level >= wpi::WPI_LOG_ERROR) {
    levelmsg = "ERROR";
  } else if (level >= wpi::WPI_LOG_WARNING) {
    levelmsg = "WARNING";
  } else {
    return;
  }
  wpi::print(stderr, "NT: {}: {} ({}:{})\n", levelmsg, msg, file, line);
}

static constexpr unsigned int kFlagCritical = 1u << 16;
static constexpr unsigned int kFlagError = 1u << 17;
static constexpr unsigned int kFlagWarning = 1u << 18;
static constexpr unsigned int kFlagInfo = 1u << 19;
static constexpr unsigned int kFlagDebug = 1u << 20;
static constexpr unsigned int kFlagDebug1 = 1u << 21;
static constexpr unsigned int kFlagDebug2 = 1u << 22;
static constexpr unsigned int kFlagDebug3 = 1u << 23;
static constexpr unsigned int kFlagDebug4 = 1u << 24;

static unsigned int LevelToFlag(unsigned int level) {
  if (level >= wpi::WPI_LOG_CRITICAL) {
    return EventFlags::kLogMessage | kFlagCritical;
  } else if (level >= wpi::WPI_LOG_ERROR) {
    return EventFlags::kLogMessage | kFlagError;
  } else if (level >= wpi::WPI_LOG_WARNING) {
    return EventFlags::kLogMessage | kFlagWarning;
  } else if (level >= wpi::WPI_LOG_INFO) {
    return EventFlags::kLogMessage | kFlagInfo;
  } else if (level >= wpi::WPI_LOG_DEBUG) {
    return EventFlags::kLogMessage | kFlagDebug;
  } else if (level >= wpi::WPI_LOG_DEBUG1) {
    return EventFlags::kLogMessage | kFlagDebug1;
  } else if (level >= wpi::WPI_LOG_DEBUG2) {
    return EventFlags::kLogMessage | kFlagDebug2;
  } else if (level >= wpi::WPI_LOG_DEBUG3) {
    return EventFlags::kLogMessage | kFlagDebug3;
  } else if (level >= wpi::WPI_LOG_DEBUG4) {
    return EventFlags::kLogMessage | kFlagDebug4;
  } else {
    return EventFlags::kLogMessage;
  }
}

static unsigned int LevelsToEventMask(unsigned int minLevel,
                                      unsigned int maxLevel) {
  unsigned int mask = 0;
  if (minLevel <= wpi::WPI_LOG_CRITICAL && maxLevel >= wpi::WPI_LOG_CRITICAL) {
    mask |= kFlagCritical;
  }
  if (minLevel <= wpi::WPI_LOG_ERROR && maxLevel >= wpi::WPI_LOG_ERROR) {
    mask |= kFlagError;
  }
  if (minLevel <= wpi::WPI_LOG_WARNING && maxLevel >= wpi::WPI_LOG_WARNING) {
    mask |= kFlagWarning;
  }
  if (minLevel <= wpi::WPI_LOG_INFO && maxLevel >= wpi::WPI_LOG_INFO) {
    mask |= kFlagInfo;
  }
  if (minLevel <= wpi::WPI_LOG_DEBUG && maxLevel >= wpi::WPI_LOG_DEBUG) {
    mask |= kFlagDebug;
  }
  if (minLevel <= wpi::WPI_LOG_DEBUG1 && maxLevel >= wpi::WPI_LOG_DEBUG1) {
    mask |= kFlagDebug1;
  }
  if (minLevel <= wpi::WPI_LOG_DEBUG2 && maxLevel >= wpi::WPI_LOG_DEBUG2) {
    mask |= kFlagDebug2;
  }
  if (minLevel <= wpi::WPI_LOG_DEBUG3 && maxLevel >= wpi::WPI_LOG_DEBUG3) {
    mask |= kFlagDebug3;
  }
  if (minLevel <= wpi::WPI_LOG_DEBUG4 && maxLevel >= wpi::WPI_LOG_DEBUG4) {
    mask |= kFlagDebug4;
  }
  if (mask == 0) {
    mask = EventFlags::kLogMessage;
  }
  return mask;
}

LoggerImpl::LoggerImpl(IListenerStorage& listenerStorage)
    : m_listenerStorage{listenerStorage} {}

LoggerImpl::~LoggerImpl() = default;

void LoggerImpl::AddListener(NT_Listener listener, unsigned int minLevel,
                             unsigned int maxLevel) {
  ++m_listenerCount;
  std::scoped_lock lock{m_mutex};
  m_listenerLevels.emplace_back(listener, minLevel, maxLevel);
  m_listenerStorage.Activate(listener, LevelsToEventMask(minLevel, maxLevel),
                             [](unsigned int mask, Event* event) {
                               event->flags = NT_EVENT_LOGMESSAGE;
                               return true;
                             });
}

void LoggerImpl::RemoveListener(NT_Listener listener) {
  --m_listenerCount;
  std::scoped_lock lock{m_mutex};
  std::erase_if(m_listenerLevels,
                [&](auto& v) { return v.listener == listener; });
}

unsigned int LoggerImpl::GetMinLevel() {
  // return 0;
  std::scoped_lock lock{m_mutex};
  unsigned int level = NT_LOG_INFO;
  for (auto&& listenerLevel : m_listenerLevels) {
    if (listenerLevel.minLevel < level) {
      level = listenerLevel.minLevel;
    }
  }
  return level;
}

void LoggerImpl::Log(unsigned int level, const char* file, unsigned int line,
                     const char* msg) {
  auto filename = fs::path{file}.filename();
  if (m_listenerCount == 0) {
    DefaultLogger(level, filename.string().c_str(), line, msg);
  } else {
    m_listenerStorage.Notify(LevelToFlag(level), level, filename.string(), line,
                             msg);
  }
}
