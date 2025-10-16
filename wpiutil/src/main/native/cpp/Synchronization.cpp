// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/Synchronization.h"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <mutex>

#include "wpi/util/DenseMap.hpp"
#include "wpi/util/SmallVector.hpp"
#include "wpi/util/UidVector.hpp"
#include "wpi/util/condition_variable.hpp"
#include "wpi/util/mutex.hpp"

using namespace wpi::util;

// Count of active threads using the handle manager singleton. A negative value
// indicates that the manager is being destroyed. When the manager is being
// destroyed, it first biases the count negative by adding INT_MIN / 2, and then
// waits for the count to return to INT_MIN / 2 before exiting.  Any active
// threads will eventually decrement the count, and new threads will see the
// negative count and re-decrement it immediately; in either case the atomic
// will be notified so the destructor can continue running.
//
// This allows us both to detect in callers if destruction is in progress (value
// < 0) and also to detect when all threads have finished.
static std::atomic_int gActive{0};

namespace {

struct State {
  int signaled{0};
  bool autoReset{false};
  wpi::util::SmallVector<wpi::util::condition_variable*, 2> waiters;
};

struct HandleManager {
  ~HandleManager() {
    gActive.fetch_add(INT_MIN / 2);

    // wake up all waiters
    {
      std::scoped_lock lock{mutex};
      for (auto&& [handle, state] : states) {
        for (auto&& waiter : state.waiters) {
          waiter->notify_all();
        }
      }
    }

    // wait for other threads to finish
    //
    // We don't use this logic on Windows, because Windows automatically kills
    // all threads on process exit, so this actually can result in deadlocks
    // because the other threads never decrement the count.
#ifndef _WIN32
    for (;;) {
      int nowActive = gActive.load();
      if (nowActive == INT_MIN / 2) {
        break;
      }
      // wait for active count to change
      gActive.wait(nowActive);
    }
#endif
  }
  wpi::util::mutex mutex;
  wpi::util::UidVector<int, 8> eventIds;
  wpi::util::UidVector<int, 8> semaphoreIds;
  wpi::util::DenseMap<WPI_Handle, State> states;
};

class ManagerGuard {
 public:
  ManagerGuard()
      : m_active{gActive.fetch_add(1, std::memory_order_acquire) >= 0} {}

  ~ManagerGuard() {
    if (gActive.fetch_sub(1, std::memory_order_release) < 0) {
      gActive.notify_all();
    }
  }

  explicit operator bool() const { return m_active; }

  HandleManager& GetManager() {
    static HandleManager manager;
    return manager;
  }

 private:
  bool m_active;
};

}  // namespace

WPI_EventHandle wpi::util::CreateEvent(bool manualReset, bool initialState) {
  ManagerGuard guard;
  if (!guard) {
    return {};
  }
  auto& manager = guard.GetManager();
  std::scoped_lock lock{manager.mutex};

  auto index = manager.eventIds.emplace_back(0);
  WPI_EventHandle handle = (kHandleTypeEvent << 24) | (index & 0xffffff);

  // configure state data
  auto& state = manager.states[handle];
  state.signaled = initialState ? 1 : 0;
  state.autoReset = !manualReset;
  return handle;
}

void wpi::util::DestroyEvent(WPI_EventHandle handle) {
  if ((handle >> 24) != kHandleTypeEvent) {
    return;
  }

  DestroySignalObject(handle);

  ManagerGuard guard;
  if (!guard) {
    return;
  }
  auto& manager = guard.GetManager();
  std::scoped_lock lock{manager.mutex};
  manager.eventIds.erase(handle & 0xffffff);
}

void wpi::util::SetEvent(WPI_EventHandle handle) {
  if ((handle >> 24) != kHandleTypeEvent) {
    return;
  }

  SetSignalObject(handle);
}

void wpi::util::ResetEvent(WPI_EventHandle handle) {
  if ((handle >> 24) != kHandleTypeEvent) {
    return;
  }

  ResetSignalObject(handle);
}

WPI_SemaphoreHandle wpi::util::CreateSemaphore(int initialCount,
                                               int maximumCount) {
  ManagerGuard guard;
  if (!guard) {
    return {};
  }
  auto& manager = guard.GetManager();
  std::scoped_lock lock{manager.mutex};

  auto index = manager.semaphoreIds.emplace_back(maximumCount);
  WPI_EventHandle handle = (kHandleTypeSemaphore << 24) | (index & 0xffffff);

  // configure state data
  auto& state = manager.states[handle];
  state.signaled = initialCount;
  state.autoReset = true;

  return handle;
}

void wpi::util::DestroySemaphore(WPI_SemaphoreHandle handle) {
  if ((handle >> 24) != kHandleTypeSemaphore) {
    return;
  }

  DestroySignalObject(handle);

  ManagerGuard guard;
  if (!guard) {
    return;
  }
  auto& manager = guard.GetManager();
  std::scoped_lock lock{manager.mutex};
  manager.eventIds.erase(handle & 0xffffff);
}

bool wpi::util::ReleaseSemaphore(WPI_SemaphoreHandle handle, int releaseCount,
                                 int* prevCount) {
  if ((handle >> 24) != kHandleTypeSemaphore) {
    return false;
  }
  if (releaseCount <= 0) {
    return false;
  }
  int index = handle & 0xffffff;

  ManagerGuard guard;
  if (!guard) {
    return true;
  }
  auto& manager = guard.GetManager();
  std::scoped_lock lock{manager.mutex};
  auto it = manager.states.find(handle);
  if (it == manager.states.end()) {
    return false;
  }
  auto& state = it->second;
  int maxCount = manager.eventIds[index];
  if (prevCount) {
    *prevCount = state.signaled;
  }
  if ((maxCount - state.signaled) < releaseCount) {
    return false;
  }
  state.signaled += releaseCount;
  for (auto& waiter : state.waiters) {
    waiter->notify_all();
  }
  return true;
}

bool wpi::util::WaitForObject(WPI_Handle handle) {
  return WaitForObject(handle, -1, nullptr);
}

bool wpi::util::WaitForObject(WPI_Handle handle, double timeout,
                              bool* timedOut) {
  WPI_Handle signaledValue;
  auto signaled = WaitForObjects(
      std::span(&handle, 1), std::span(&signaledValue, 1), timeout, timedOut);
  if (signaled.empty()) {
    return false;
  }
  return (signaled[0] & 0x80000000ul) == 0;
}

std::span<WPI_Handle> wpi::util::WaitForObjects(
    std::span<const WPI_Handle> handles, std::span<WPI_Handle> signaled) {
  return WaitForObjects(handles, signaled, -1, nullptr);
}

std::span<WPI_Handle> wpi::util::WaitForObjects(
    std::span<const WPI_Handle> handles, std::span<WPI_Handle> signaled,
    double timeout, bool* timedOut) {
  ManagerGuard guard;
  if (!guard) {
    if (timedOut) {
      *timedOut = false;
    }
    return {};
  }
  auto& manager = guard.GetManager();
  std::unique_lock lock{manager.mutex};
  wpi::util::condition_variable cv;
  bool addedWaiters = false;
  bool timedOutVal = false;
  size_t count = 0;

  for (;;) {
    for (auto handle : handles) {
      auto it = manager.states.find(handle);
      if (it == manager.states.end()) {
        if (count < signaled.size()) {
          // treat a non-existent handle as signaled, but set the error bit
          signaled[count++] = handle | 0x80000000ul;
        }
      } else {
        auto& state = it->second;
        if (state.signaled > 0) {
          if (count < signaled.size()) {
            signaled[count++] = handle;
          }
          if (state.autoReset) {
            --state.signaled;
            if (state.signaled < 0) {
              state.signaled = 0;
            }
          }
        }
      }
    }

    if (timedOutVal || count != 0) {
      break;
    }

    if (timeout == 0) {
      timedOutVal = true;
      break;
    }

    if (!addedWaiters) {
      addedWaiters = true;
      for (auto handle : handles) {
        auto& state = manager.states[handle];
        state.waiters.emplace_back(&cv);
      }
    }

    if (gActive.load(std::memory_order_acquire) < 0) {
      // shutting down
      timedOutVal = false;
      count = 0;
      break;
    }

    if (timeout < 0) {
      cv.wait(lock);
    } else {
      auto timeoutTime = std::chrono::steady_clock::now() +
                         std::chrono::duration<double>(timeout);
      if (cv.wait_until(lock, timeoutTime) == std::cv_status::timeout) {
        timedOutVal = true;
      }
    }

    if (gActive.load(std::memory_order_acquire) < 0) {
      // shutting down
      timedOutVal = false;
      count = 0;
      break;
    }
  }

  if (addedWaiters) {
    for (auto handle : handles) {
      auto& state = manager.states[handle];
      auto it = std::find(state.waiters.begin(), state.waiters.end(), &cv);
      if (it != state.waiters.end()) {
        state.waiters.erase(it);
      }
    }
  }

  if (timedOut) {
    *timedOut = timedOutVal;
  }

  return signaled.subspan(0, count);
}

void wpi::util::CreateSignalObject(WPI_Handle handle, bool manualReset,
                                   bool initialState) {
  ManagerGuard guard;
  if (!guard) {
    return;
  }
  auto& manager = guard.GetManager();
  std::scoped_lock lock{manager.mutex};
  auto& state = manager.states[handle];
  state.signaled = initialState ? 1 : 0;
  state.autoReset = !manualReset;
}

void wpi::util::SetSignalObject(WPI_Handle handle) {
  ManagerGuard guard;
  if (!guard) {
    return;
  }
  auto& manager = guard.GetManager();
  std::scoped_lock lock{manager.mutex};
  auto it = manager.states.find(handle);
  if (it == manager.states.end()) {
    return;
  }
  auto& state = it->second;
  state.signaled = 1;
  for (auto& waiter : state.waiters) {
    waiter->notify_all();
    if (state.autoReset) {
      // expect the first waiter to reset it
      break;
    }
  }
}

void wpi::util::ResetSignalObject(WPI_Handle handle) {
  ManagerGuard guard;
  if (!guard) {
    return;
  }
  auto& manager = guard.GetManager();
  std::scoped_lock lock{manager.mutex};
  auto it = manager.states.find(handle);
  if (it != manager.states.end()) {
    it->second.signaled = 0;
  }
}

void wpi::util::DestroySignalObject(WPI_Handle handle) {
  ManagerGuard guard;
  if (!guard) {
    return;
  }
  auto& manager = guard.GetManager();
  std::scoped_lock lock{manager.mutex};

  auto it = manager.states.find(handle);
  if (it != manager.states.end()) {
    // wake up any waiters
    for (auto& waiter : it->second.waiters) {
      waiter->notify_all();
    }
    manager.states.erase(it);
  }
}

extern "C" {

WPI_EventHandle WPI_CreateEvent(int manual_reset, int initial_state) {
  return wpi::util::CreateEvent(manual_reset != 0, initial_state != 0);
}

void WPI_DestroyEvent(WPI_EventHandle handle) {
  wpi::util::DestroyEvent(handle);
}

void WPI_SetEvent(WPI_EventHandle handle) {
  wpi::util::SetEvent(handle);
}

void WPI_ResetEvent(WPI_EventHandle handle) {
  wpi::util::ResetEvent(handle);
}

WPI_SemaphoreHandle WPI_CreateSemaphore(int initial_count, int maximum_count) {
  return wpi::util::CreateSemaphore(initial_count, maximum_count);
}

void WPI_DestroySemaphore(WPI_SemaphoreHandle handle) {
  wpi::util::DestroySemaphore(handle);
}

int WPI_ReleaseSemaphore(WPI_SemaphoreHandle handle, int release_count,
                         int* prev_count) {
  return wpi::util::ReleaseSemaphore(handle, release_count, prev_count);
}

int WPI_WaitForObject(WPI_Handle handle) {
  return wpi::util::WaitForObject(handle);
}

int WPI_WaitForObjectTimeout(WPI_Handle handle, double timeout,
                             int* timed_out) {
  bool timedOutBool;
  int rv = wpi::util::WaitForObject(handle, timeout, &timedOutBool);
  *timed_out = timedOutBool ? 1 : 0;
  return rv;
}

int WPI_WaitForObjects(const WPI_Handle* handles, int handles_count,
                       WPI_Handle* signaled) {
  return wpi::util::WaitForObjects(std::span(handles, handles_count),
                                   std::span(signaled, handles_count))
      .size();
}

int WPI_WaitForObjectsTimeout(const WPI_Handle* handles, int handles_count,
                              WPI_Handle* signaled, double timeout,
                              int* timed_out) {
  bool timedOutBool;
  auto signaledResult = wpi::util::WaitForObjects(
      std::span(handles, handles_count), std::span(signaled, handles_count),
      timeout, &timedOutBool);
  *timed_out = timedOutBool ? 1 : 0;
  return signaledResult.size();
}

void WPI_CreateSignalObject(WPI_Handle handle, int manual_reset,
                            int initial_state) {
  wpi::util::CreateSignalObject(handle, manual_reset, initial_state);
}

void WPI_SetSignalObject(WPI_Handle handle) {
  wpi::util::SetSignalObject(handle);
}

void WPI_ResetSignalObject(WPI_Handle handle) {
  wpi::util::ResetSignalObject(handle);
}

void WPI_DestroySignalObject(WPI_Handle handle) {
  wpi::util::DestroySignalObject(handle);
}

}  // extern "C"
