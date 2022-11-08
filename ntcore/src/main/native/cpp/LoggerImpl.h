// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <vector>

#include <wpi/mutex.h>

#include "IListenerStorage.h"
#include "ntcore_c.h"
#include "ntcore_cpp.h"

namespace nt {

class IListenerStorage;

class LoggerImpl {
 public:
  explicit LoggerImpl(IListenerStorage& listenerStorage);
  LoggerImpl(const LoggerImpl&) = delete;
  LoggerImpl& operator=(const LoggerImpl&) = delete;
  ~LoggerImpl();

  void AddListener(NT_Listener listener, unsigned int minLevel,
                   unsigned int maxLevel);
  void RemoveListener(NT_Listener listener);

  unsigned int GetMinLevel();

  void Log(unsigned int level, const char* file, unsigned int line,
           const char* msg);

 private:
  IListenerStorage& m_listenerStorage;
  std::atomic_int m_listenerCount{0};
  wpi::mutex m_mutex;

  struct ListenerLevels {
    ListenerLevels(NT_Listener listener, unsigned int minLevel,
                   unsigned int maxLevel)
        : listener{listener}, minLevel{minLevel}, maxLevel{maxLevel} {}

    NT_Listener listener;
    unsigned int minLevel;
    unsigned int maxLevel;
  };
  std::vector<ListenerLevels> m_listenerLevels;
};

}  // namespace nt
