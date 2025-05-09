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

static constexpr unsigned int FLAG_CRITICAL = 1u << 16;
static constexpr unsigned int FLAG_ERROR = 1u << 17;
static constexpr unsigned int FLAG_WARNING = 1u << 18;
static constexpr unsigned int FLAG_INFO = 1u << 19;
static constexpr unsigned int FLAG_DEBUG = 1u << 20;
static constexpr unsigned int FLAG_DEBUG1 = 1u << 21;
static constexpr unsigned int FLAG_DEBUG2 = 1u << 22;
static constexpr unsigned int FLAG_DEBUG3 = 1u << 23;
static constexpr unsigned int FLAG_DEBUG4 = 1u << 24;

static unsigned int LevelToFlag(unsigned int level) {
  if (level >= wpi::WPI_LOG_CRITICAL) {
    return EventFlags::LOG_MESSAGE | FLAG_CRITICAL;
  } else if (level >= wpi::WPI_LOG_ERROR) {
    return EventFlags::LOG_MESSAGE | FLAG_ERROR;
  } else if (level >= wpi::WPI_LOG_WARNING) {
    return EventFlags::LOG_MESSAGE | FLAG_WARNING;
  } else if (level >= wpi::WPI_LOG_INFO) {
    return EventFlags::LOG_MESSAGE | FLAG_INFO;
  } else if (level >= wpi::WPI_LOG_DEBUG) {
    return EventFlags::LOG_MESSAGE | FLAG_DEBUG;
  } else if (level >= wpi::WPI_LOG_DEBUG1) {
    return EventFlags::LOG_MESSAGE | FLAG_DEBUG1;
  } else if (level >= wpi::WPI_LOG_DEBUG2) {
    return EventFlags::LOG_MESSAGE | FLAG_DEBUG2;
  } else if (level >= wpi::WPI_LOG_DEBUG3) {
    return EventFlags::LOG_MESSAGE | FLAG_DEBUG3;
  } else if (level >= wpi::WPI_LOG_DEBUG4) {
    return EventFlags::LOG_MESSAGE | FLAG_DEBUG4;
  } else {
    return EventFlags::LOG_MESSAGE;
  }
}

static unsigned int LevelsToEventMask(unsigned int minLevel,
                                      unsigned int maxLevel) {
  unsigned int mask = 0;
  if (minLevel <= wpi::WPI_LOG_CRITICAL && maxLevel >= wpi::WPI_LOG_CRITICAL) {
    mask |= FLAG_CRITICAL;
  }
  if (minLevel <= wpi::WPI_LOG_ERROR && maxLevel >= wpi::WPI_LOG_ERROR) {
    mask |= FLAG_ERROR;
  }
  if (minLevel <= wpi::WPI_LOG_WARNING && maxLevel >= wpi::WPI_LOG_WARNING) {
    mask |= FLAG_WARNING;
  }
  if (minLevel <= wpi::WPI_LOG_INFO && maxLevel >= wpi::WPI_LOG_INFO) {
    mask |= FLAG_INFO;
  }
  if (minLevel <= wpi::WPI_LOG_DEBUG && maxLevel >= wpi::WPI_LOG_DEBUG) {
    mask |= FLAG_DEBUG;
  }
  if (minLevel <= wpi::WPI_LOG_DEBUG1 && maxLevel >= wpi::WPI_LOG_DEBUG1) {
    mask |= FLAG_DEBUG1;
  }
  if (minLevel <= wpi::WPI_LOG_DEBUG2 && maxLevel >= wpi::WPI_LOG_DEBUG2) {
    mask |= FLAG_DEBUG2;
  }
  if (minLevel <= wpi::WPI_LOG_DEBUG3 && maxLevel >= wpi::WPI_LOG_DEBUG3) {
    mask |= FLAG_DEBUG3;
  }
  if (minLevel <= wpi::WPI_LOG_DEBUG4 && maxLevel >= wpi::WPI_LOG_DEBUG4) {
    mask |= FLAG_DEBUG4;
  }
  if (mask == 0) {
    mask = EventFlags::LOG_MESSAGE;
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
