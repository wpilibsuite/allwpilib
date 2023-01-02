// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/GlobalState.h"

#include <map>

#include "wpi/mutex.h"

using namespace wpi;

#ifndef __FRC_ROBORIO__

namespace {

struct GlobalStateImpl {
  void Register(int priority, std::function<void()> fn) {
    std::scoped_lock lock{m_map_mutex};
    m_functions.emplace(priority, std::move(fn));
  }

  void Reset() {
    std::multimap<int, wpi::impl::ResetCallback> functions;
    {
      std::scoped_lock lock{m_map_mutex};
      functions = m_functions;
    }

    {
      // ensures that reset functions are never called concurrently, and
      // that accidental calls to the Register function won't deadlock
      std::scoped_lock lock{m_call_mutex};
      for (const auto& kv : functions) {
        kv.second();
      }
    }
  }

  wpi::mutex m_map_mutex;
  wpi::mutex m_call_mutex;
  std::multimap<int, wpi::impl::ResetCallback> m_functions;
};

static GlobalStateImpl& GetGlobalState() {
  static GlobalStateImpl* impl = new GlobalStateImpl();
  return *impl;
}

}  // namespace

void ResetGlobalState() {
  GetGlobalState().Reset();
}

void impl::RegisterGlobalStateReset(int priority, std::function<void()> fn) {
  GetGlobalState().Register(priority, std::move(fn));
}

#else  // !__FRC_ROBORIO__

void ResetGlobalState() {}
void impl::RegisterGlobalStateReset(int priority, std::function<void()> fn) {}

#endif
