// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

#include "Handle.hpp"
#include "HandleMap.hpp"
#include "IListenerStorage.hpp"
#include "VectorSet.hpp"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/DenseMap.hpp"
#include "wpi/util/SafeThread.hpp"
#include "wpi/util/SmallVector.hpp"
#include "wpi/util/Synchronization.h"
#include "wpi/util/mutex.hpp"

namespace nt {

class ListenerStorage final : public IListenerStorage {
 public:
  explicit ListenerStorage(int inst) : m_inst{inst} {}
  ListenerStorage(const ListenerStorage&) = delete;
  ListenerStorage& operator=(const ListenerStorage&) = delete;

  // IListenerStorage interface
  void Activate(NT_Listener listenerHandle, unsigned int mask,
                FinishEventFunc finishEvent = {}) final;
  void Notify(std::span<const NT_Listener> handles, unsigned int flags,
              std::span<ConnectionInfo const* const> infos) final;
  void Notify(std::span<const NT_Listener> handles, unsigned int flags,
              std::span<const TopicInfo> infos) final;
  void Notify(std::span<const NT_Listener> handles, unsigned int flags,
              NT_Topic topic, NT_Handle subentry, const Value& value) final;
  void Notify(unsigned int flags, unsigned int level, std::string_view filename,
              unsigned int line, std::string_view message) final;
  void NotifyTimeSync(std::span<const NT_Listener> handles, unsigned int flags,
                      int64_t serverTimeOffset, int64_t rtt2, bool valid) final;

  // user-facing functions
  NT_Listener AddListener(ListenerCallback callback);
  NT_Listener AddListener(NT_ListenerPoller pollerHandle);
  NT_ListenerPoller CreateListenerPoller();

  // returns listener handle and mask for each listener that was destroyed
  [[nodiscard]]
  std::vector<std::pair<NT_Listener, unsigned int>> DestroyListenerPoller(
      NT_ListenerPoller pollerHandle);

  std::vector<Event> ReadListenerQueue(NT_ListenerPoller pollerHandle);

  // returns listener handle and mask for each listener that was destroyed
  [[nodiscard]]
  std::vector<std::pair<NT_Listener, unsigned int>> RemoveListener(
      NT_Listener listenerHandle);

  bool WaitForListenerQueue(double timeout);

  void Reset();

 private:
  // these assume the mutex is already held
  NT_Listener DoAddListener(NT_ListenerPoller pollerHandle);
  std::vector<std::pair<NT_Listener, unsigned int>> DoRemoveListeners(
      std::span<const NT_Listener> handles);

  int m_inst;
  mutable wpi::mutex m_mutex;

  struct PollerData {
    static constexpr auto kType = Handle::kListenerPoller;

    explicit PollerData(NT_ListenerPoller handle) : handle{handle} {}

    wpi::SignalObject<NT_ListenerPoller> handle;
    std::vector<Event> queue;
  };
  HandleMap<PollerData, 8> m_pollers;

  struct ListenerData {
    static constexpr auto kType = Handle::kListener;

    ListenerData(NT_Listener handle, PollerData* poller)
        : handle{handle}, poller{poller} {}

    wpi::SignalObject<NT_Listener> handle;
    PollerData* poller;
    wpi::SmallVector<std::pair<FinishEventFunc, unsigned int>, 2> sources;
    unsigned int eventMask{0};
  };
  HandleMap<ListenerData, 8> m_listeners;

  VectorSet<ListenerData*> m_connListeners;
  VectorSet<ListenerData*> m_topicListeners;
  VectorSet<ListenerData*> m_valueListeners;
  VectorSet<ListenerData*> m_logListeners;
  VectorSet<ListenerData*> m_timeSyncListeners;

  class Thread final : public wpi::SafeThreadEvent {
   public:
    explicit Thread(NT_ListenerPoller poller) : m_poller{poller} {}

    void Main() final;

    NT_ListenerPoller m_poller;
    wpi::DenseMap<NT_Listener, ListenerCallback> m_callbacks;
    wpi::Event m_waitQueueWakeup;
    wpi::Event m_waitQueueWaiter;
  };
  wpi::SafeThreadOwner<Thread> m_thread;
};

}  // namespace nt
