// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/Synchronization.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
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

constexpr size_t STATE_SHARD_COUNT = 32;

class SignalWaiter {
 public:
  void Notify() {
    {
      std::scoped_lock lock{m_mutex};
      m_notified = true;
    }
    m_cv.notify_all();
  }

  void Wait() {
    std::unique_lock lock{m_mutex};
    m_cv.wait(lock, [&] { return m_notified; });
    m_notified = false;
  }

  bool WaitFor(double timeout) {
    std::unique_lock lock{m_mutex};
    auto timeoutTime = std::chrono::steady_clock::now() +
                       std::chrono::duration<double>{timeout};
    if (!m_cv.wait_until(lock, timeoutTime, [&] { return m_notified; })) {
      return true;
    }
    m_notified = false;
    return false;
  }

 private:
  wpi::util::mutex m_mutex;
  wpi::util::condition_variable m_cv;
  bool m_notified{false};
};

struct State {
  int signaled{0};
  int maxCount{0};
  bool autoReset{false};
  wpi::util::SmallVector<SignalWaiter*, 2> waiters;
};

struct StateShard {
  wpi::util::mutex mutex;
  wpi::util::DenseMap<WPI_Handle, State> states;
};

size_t GetStateShardIndex(WPI_Handle handle) {
  auto value = static_cast<uint32_t>(handle);
  value ^= value >> 16;
  value *= 0x7feb352dU;
  value ^= value >> 15;
  return value % STATE_SHARD_COUNT;
}

struct HandleManager {
  ~HandleManager() {
    gActive.fetch_add(INT_MIN / 2);

    // wake up all waiters
    for (auto& shard : stateShards) {
      std::scoped_lock lock{shard.mutex};
      for (auto&& [handle, state] : shard.states) {
        for (auto&& waiter : state.waiters) {
          waiter->Notify();
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
  StateShard& GetStateShard(WPI_Handle handle) {
    return stateShards[GetStateShardIndex(handle)];
  }

  wpi::util::mutex idsMutex;
  wpi::util::UidVector<int, 8> eventIds;
  wpi::util::UidVector<int, 8> semaphoreIds;
  std::array<StateShard, STATE_SHARD_COUNT> stateShards;
};

using StateShardIndices = wpi::util::SmallVector<size_t, 8>;
using StateShardLocks =
    wpi::util::SmallVector<std::unique_lock<wpi::util::mutex>, 8>;

StateShardIndices GetStateShardIndices(std::span<const WPI_Handle> handles) {
  StateShardIndices indices;
  for (auto handle : handles) {
    indices.emplace_back(GetStateShardIndex(handle));
  }

  std::sort(indices.begin(), indices.end());

  StateShardIndices uniqueIndices;
  for (auto index : indices) {
    if (uniqueIndices.empty() || uniqueIndices.back() != index) {
      uniqueIndices.emplace_back(index);
    }
  }
  return uniqueIndices;
}

StateShardLocks LockStateShards(HandleManager& manager,
                                const StateShardIndices& shardIndices) {
  StateShardLocks locks;
  for (auto index : shardIndices) {
    locks.emplace_back(manager.stateShards[index].mutex);
  }
  return locks;
}

State* FindState(HandleManager& manager, WPI_Handle handle) {
  auto& shard = manager.GetStateShard(handle);
  auto it = shard.states.find(handle);
  if (it == shard.states.end()) {
    return nullptr;
  }
  return &it->second;
}

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

WPI_EventHandle wpi::util::MakeEvent(bool manualReset, bool initialState) {
  ManagerGuard guard;
  if (!guard) {
    return {};
  }
  auto& manager = guard.GetManager();

  size_t index;
  {
    std::scoped_lock lock{manager.idsMutex};
    index = manager.eventIds.emplace_back(0);
  }
  WPI_EventHandle handle = (HANDLE_TYPE_EVENT << 24) | (index & 0xffffff);

  // configure state data
  auto& shard = manager.GetStateShard(handle);
  std::scoped_lock lock{shard.mutex};
  auto& state = shard.states[handle];
  state.signaled = initialState ? 1 : 0;
  state.autoReset = !manualReset;
  return handle;
}

void wpi::util::DestroyEvent(WPI_EventHandle handle) {
  if ((handle >> 24) != HANDLE_TYPE_EVENT) {
    return;
  }

  DestroySignalObject(handle);

  ManagerGuard guard;
  if (!guard) {
    return;
  }
  auto& manager = guard.GetManager();
  std::scoped_lock lock{manager.idsMutex};
  manager.eventIds.erase(handle & 0xffffff);
}

void wpi::util::SetEvent(WPI_EventHandle handle) {
  if ((handle >> 24) != HANDLE_TYPE_EVENT) {
    return;
  }

  SetSignalObject(handle);
}

void wpi::util::ResetEvent(WPI_EventHandle handle) {
  if ((handle >> 24) != HANDLE_TYPE_EVENT) {
    return;
  }

  ResetSignalObject(handle);
}

WPI_SemaphoreHandle wpi::util::MakeSemaphore(int initialCount,
                                             int maximumCount) {
  ManagerGuard guard;
  if (!guard) {
    return {};
  }
  auto& manager = guard.GetManager();

  size_t index;
  {
    std::scoped_lock lock{manager.idsMutex};
    index = manager.semaphoreIds.emplace_back(maximumCount);
  }
  WPI_EventHandle handle = (HANDLE_TYPE_SEMAPHORE << 24) | (index & 0xffffff);

  // configure state data
  auto& shard = manager.GetStateShard(handle);
  std::scoped_lock lock{shard.mutex};
  auto& state = shard.states[handle];
  state.signaled = initialCount;
  state.maxCount = maximumCount;
  state.autoReset = true;

  return handle;
}

void wpi::util::DestroySemaphore(WPI_SemaphoreHandle handle) {
  if ((handle >> 24) != HANDLE_TYPE_SEMAPHORE) {
    return;
  }

  DestroySignalObject(handle);

  ManagerGuard guard;
  if (!guard) {
    return;
  }
  auto& manager = guard.GetManager();
  std::scoped_lock lock{manager.idsMutex};
  manager.semaphoreIds.erase(handle & 0xffffff);
}

bool wpi::util::ReleaseSemaphore(WPI_SemaphoreHandle handle, int releaseCount,
                                 int* prevCount) {
  if ((handle >> 24) != HANDLE_TYPE_SEMAPHORE) {
    return false;
  }
  if (releaseCount <= 0) {
    return false;
  }

  ManagerGuard guard;
  if (!guard) {
    return true;
  }
  auto& manager = guard.GetManager();
  auto& shard = manager.GetStateShard(handle);
  std::scoped_lock lock{shard.mutex};
  auto it = shard.states.find(handle);
  if (it == shard.states.end()) {
    return false;
  }
  auto& state = it->second;
  if (prevCount) {
    *prevCount = state.signaled;
  }
  if ((state.maxCount - state.signaled) < releaseCount) {
    return false;
  }
  state.signaled += releaseCount;
  for (auto& waiter : state.waiters) {
    waiter->Notify();
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
  auto shardIndices = GetStateShardIndices(handles);
  auto locks = LockStateShards(manager, shardIndices);
  SignalWaiter waiter;
  bool addedWaiters = false;
  bool timedOutVal = false;
  size_t count = 0;

  for (;;) {
    count = 0;
    for (auto handle : handles) {
      auto state = FindState(manager, handle);
      if (!state) {
        if (count < signaled.size()) {
          // treat a non-existent handle as signaled, but set the error bit
          signaled[count++] = handle | 0x80000000ul;
        }
      } else {
        if (state->signaled > 0) {
          if (count < signaled.size()) {
            signaled[count++] = handle;
          }
          if (state->autoReset) {
            --state->signaled;
            if (state->signaled < 0) {
              state->signaled = 0;
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
        auto state = FindState(manager, handle);
        if (state) {
          state->waiters.emplace_back(&waiter);
        }
      }
    }

    if (gActive.load(std::memory_order_acquire) < 0) {
      // shutting down
      timedOutVal = false;
      count = 0;
      break;
    }

    locks.clear();
    if (timeout < 0) {
      waiter.Wait();
    } else if (waiter.WaitFor(timeout)) {
      timedOutVal = true;
    }
    locks = LockStateShards(manager, shardIndices);

    if (gActive.load(std::memory_order_acquire) < 0) {
      // shutting down
      timedOutVal = false;
      count = 0;
      break;
    }
  }

  if (addedWaiters) {
    for (auto handle : handles) {
      auto state = FindState(manager, handle);
      if (state) {
        auto it =
            std::find(state->waiters.begin(), state->waiters.end(), &waiter);
        if (it != state->waiters.end()) {
          state->waiters.erase(it);
        }
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
  auto& shard = manager.GetStateShard(handle);
  std::scoped_lock lock{shard.mutex};
  auto& state = shard.states[handle];
  state.signaled = initialState ? 1 : 0;
  state.autoReset = !manualReset;
}

void wpi::util::SetSignalObject(WPI_Handle handle) {
  ManagerGuard guard;
  if (!guard) {
    return;
  }
  auto& manager = guard.GetManager();
  auto& shard = manager.GetStateShard(handle);
  std::scoped_lock lock{shard.mutex};
  auto it = shard.states.find(handle);
  if (it == shard.states.end()) {
    return;
  }
  auto& state = it->second;
  state.signaled = 1;
  for (auto& waiter : state.waiters) {
    waiter->Notify();
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
  auto& shard = manager.GetStateShard(handle);
  std::scoped_lock lock{shard.mutex};
  auto it = shard.states.find(handle);
  if (it != shard.states.end()) {
    it->second.signaled = 0;
  }
}

void wpi::util::DestroySignalObject(WPI_Handle handle) {
  ManagerGuard guard;
  if (!guard) {
    return;
  }
  auto& manager = guard.GetManager();
  auto& shard = manager.GetStateShard(handle);
  std::scoped_lock lock{shard.mutex};

  auto it = shard.states.find(handle);
  if (it != shard.states.end()) {
    // wake up any waiters
    for (auto& waiter : it->second.waiters) {
      waiter->Notify();
    }
    shard.states.erase(it);
  }
}

extern "C" {

WPI_EventHandle WPI_MakeEvent(int manual_reset, int initial_state) {
  return wpi::util::MakeEvent(manual_reset != 0, initial_state != 0);
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

WPI_SemaphoreHandle WPI_MakeSemaphore(int initial_count, int maximum_count) {
  return wpi::util::MakeSemaphore(initial_count, maximum_count);
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
  if (timed_out) {
    *timed_out = timedOutBool ? 1 : 0;
  }
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
  if (timed_out) {
    *timed_out = timedOutBool ? 1 : 0;
  }
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
