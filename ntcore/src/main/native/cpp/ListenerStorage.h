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

#include <wpi/SafeThread.h>
#include <wpi/SmallVector.h>
#include <wpi/Synchronization.h>
#include <wpi/mutex.h>

#include "Handle.h"
#include "HandleMap.h"
#include "IListenerStorage.h"
#include "ntcore_cpp.h"

namespace nt {

class ListenerStorage final : public IListenerStorage {
 public:
  explicit ListenerStorage(int inst);
  ListenerStorage(const ListenerStorage&) = delete;
  ListenerStorage& operator=(const ListenerStorage&) = delete;
  ~ListenerStorage() final;

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

  // user-facing functions
  NT_Listener AddListener(ListenerCallback callback);
  NT_Listener AddListener(NT_ListenerPoller pollerHandle);
  NT_ListenerPoller CreateListenerPoller();

  // returns listener handle and mask for each listener that was destroyed
  [[nodiscard]] std::vector<std::pair<NT_Listener, unsigned int>>
  DestroyListenerPoller(NT_ListenerPoller pollerHandle);

  std::vector<Event> ReadListenerQueue(NT_ListenerPoller pollerHandle);

  // returns listener handle and mask for each listener that was destroyed
  [[nodiscard]] std::vector<std::pair<NT_Listener, unsigned int>>
  RemoveListener(NT_Listener listenerHandle);

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

  // Utility wrapper for making a set-like vector
  template <typename T>
  class VectorSet : public std::vector<T> {
   public:
    void Add(T value) { this->push_back(value); }
    void Remove(T value) { std::erase(*this, value); }
  };

  VectorSet<ListenerData*> m_connListeners;
  VectorSet<ListenerData*> m_topicListeners;
  VectorSet<ListenerData*> m_valueListeners;
  VectorSet<ListenerData*> m_logListeners;

  class Thread;
  wpi::SafeThreadOwner<Thread> m_thread;
};

}  // namespace nt
