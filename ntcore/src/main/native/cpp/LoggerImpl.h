// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <utility>
#include <vector>

#include <wpi/SafeThread.h>
#include <wpi/Synchronization.h>
#include <wpi/mutex.h>

#include "Handle.h"
#include "HandleMap.h"
#include "ntcore_c.h"
#include "ntcore_cpp.h"

namespace nt {

class LoggerImpl {
 public:
  explicit LoggerImpl(int inst);
  ~LoggerImpl();

  NT_Logger Add(std::function<void(const LogMessage& msg)> callback,
                unsigned int minLevel, unsigned int maxLevel);

  NT_LoggerPoller CreatePoller();
  void DestroyPoller(NT_LoggerPoller pollerHandle);
  NT_Logger AddPolled(NT_LoggerPoller pollerHandle, unsigned int minLevel,
                      unsigned int maxLevel);
  std::vector<LogMessage> ReadQueue(NT_LoggerPoller pollerHandle);
  void Remove(NT_Logger listenerHandle);

  unsigned int GetMinLevel();

  void Log(unsigned int level, const char* file, unsigned int line,
           const char* msg);

 private:
  int m_inst;
  mutable wpi::mutex m_mutex;

  struct PollerData {
    static constexpr auto kType = Handle::kLoggerPoller;

    explicit PollerData(NT_LoggerPoller handle) : handle{handle} {}

    wpi::SignalObject<NT_LoggerPoller> handle;
    std::vector<LogMessage> queue;
  };
  HandleMap<PollerData, 8> m_pollers;

  struct ListenerData {
    static constexpr auto kType = Handle::kLogger;

    ListenerData(NT_Logger handle, PollerData* poller, unsigned int minLevel,
                 unsigned int maxLevel)
        : handle{handle},
          poller{poller},
          minLevel{minLevel},
          maxLevel{maxLevel} {}

    wpi::SignalObject<NT_Logger> handle;
    PollerData* poller;
    unsigned int minLevel;
    unsigned int maxLevel;
  };
  HandleMap<ListenerData, 8> m_listeners;

  class Thread;
  wpi::SafeThreadOwner<Thread> m_thread;
};

}  // namespace nt
