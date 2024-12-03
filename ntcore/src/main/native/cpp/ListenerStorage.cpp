// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ListenerStorage.h"

#include <algorithm>
#include <utility>
#include <vector>

#include <wpi/SmallVector.h>

#include "ntcore_c.h"

using namespace nt;

void ListenerStorage::Thread::Main() {
  while (m_active) {
    WPI_Handle signaledBuf[3];
    auto signaled = wpi::WaitForObjects(
        {m_poller, m_stopEvent.GetHandle(), m_waitQueueWakeup.GetHandle()},
        signaledBuf);
    if (signaled.empty() || !m_active) {
      return;
    }
    // call all the way back out to the C++ API to ensure valid handle
    auto events = nt::ReadListenerQueue(m_poller);
    if (!events.empty()) {
      std::unique_lock lock{m_mutex};
      for (auto&& event : events) {
        auto callbackIt = m_callbacks.find(event.listener);
        if (callbackIt != m_callbacks.end()) {
          auto callback = callbackIt->second;
          lock.unlock();
          callback(event);
          lock.lock();
        }
      }
    }
    if (std::find(signaled.begin(), signaled.end(),
                  m_waitQueueWakeup.GetHandle()) != signaled.end()) {
      m_waitQueueWaiter.Set();
    }
  }
}

void ListenerStorage::Activate(NT_Listener listenerHandle, unsigned int mask,
                               FinishEventFunc finishEvent) {
  std::scoped_lock lock{m_mutex};
  if (auto listener = m_listeners.Get(listenerHandle)) {
    listener->sources.emplace_back(std::move(finishEvent), mask);
    unsigned int deltaMask = mask & (~listener->eventMask);
    listener->eventMask |= mask;

    if ((deltaMask & NT_EVENT_CONNECTION) != 0) {
      m_connListeners.Add(listener);
    }
    if ((deltaMask & NT_EVENT_TOPIC) != 0) {
      m_topicListeners.Add(listener);
    }
    if ((deltaMask & NT_EVENT_VALUE_ALL) != 0) {
      m_valueListeners.Add(listener);
    }
    // detect the higher log bits too; see LoggerImpl
    if ((deltaMask & NT_EVENT_LOGMESSAGE) != 0 ||
        (deltaMask & 0x1ff0000) != 0) {
      m_logListeners.Add(listener);
    }
    if ((deltaMask & NT_EVENT_TIMESYNC) != 0) {
      m_timeSyncListeners.Add(listener);
    }
  }
}

void ListenerStorage::Notify(std::span<const NT_Listener> handles,
                             unsigned int flags,
                             std::span<ConnectionInfo const* const> infos) {
  if (flags == 0) {
    return;
  }
  std::scoped_lock lock{m_mutex};

  auto doSignal = [&](ListenerData& listener) {
    if ((flags & listener.eventMask) != 0) {
      for (auto&& [finishEvent, mask] : listener.sources) {
        if ((flags & mask) != 0) {
          for (auto&& info : infos) {
            listener.poller->queue.emplace_back(listener.handle, flags, *info);
            // finishEvent is never set (see ConnectionList)
          }
        }
      }
      listener.handle.Set();
      listener.poller->handle.Set();
    }
  };

  if (!handles.empty()) {
    for (auto handle : handles) {
      if (auto listener = m_listeners.Get(handle)) {
        doSignal(*listener);
      }
    }
  } else {
    for (auto&& listener : m_connListeners) {
      doSignal(*listener);
    }
  }
}

void ListenerStorage::Notify(std::span<const NT_Listener> handles,
                             unsigned int flags,
                             std::span<const TopicInfo> infos) {
  if (flags == 0) {
    return;
  }
  std::scoped_lock lock{m_mutex};

  auto doSignal = [&](ListenerData& listener) {
    if ((flags & listener.eventMask) != 0) {
      int count = 0;
      for (auto&& [finishEvent, mask] : listener.sources) {
        if ((flags & mask) != 0) {
          for (auto&& info : infos) {
            listener.poller->queue.emplace_back(listener.handle, flags, info);
            if (finishEvent &&
                !finishEvent(mask, &listener.poller->queue.back())) {
              listener.poller->queue.pop_back();
            } else {
              ++count;
            }
          }
        }
      }
      if (count > 0) {
        listener.handle.Set();
        listener.poller->handle.Set();
      }
    }
  };

  if (!handles.empty()) {
    for (auto handle : handles) {
      if (auto listener = m_listeners.Get(handle)) {
        doSignal(*listener);
      }
    }
  } else {
    for (auto&& listener : m_topicListeners) {
      doSignal(*listener);
    }
  }
}

void ListenerStorage::Notify(std::span<const NT_Listener> handles,
                             unsigned int flags, NT_Topic topic,
                             NT_Handle subentry, const Value& value) {
  if (flags == 0) {
    return;
  }
  std::scoped_lock lock{m_mutex};

  auto doSignal = [&](ListenerData& listener) {
    if ((flags & listener.eventMask) != 0) {
      int count = 0;
      for (auto&& [finishEvent, mask] : listener.sources) {
        if ((flags & mask) != 0) {
          listener.poller->queue.emplace_back(listener.handle, flags, topic,
                                              subentry, value);
          if (finishEvent &&
              !finishEvent(mask, &listener.poller->queue.back())) {
            listener.poller->queue.pop_back();
          } else {
            ++count;
          }
        }
      }
      if (count > 0) {
        listener.handle.Set();
        listener.poller->handle.Set();
      }
    }
  };

  if (!handles.empty()) {
    for (auto handle : handles) {
      if (auto listener = m_listeners.Get(handle)) {
        doSignal(*listener);
      }
    }
  } else {
    for (auto&& listener : m_valueListeners) {
      doSignal(*listener);
    }
  }
}

void ListenerStorage::Notify(unsigned int flags, unsigned int level,
                             std::string_view filename, unsigned int line,
                             std::string_view message) {
  if (flags == 0) {
    return;
  }
  std::scoped_lock lock{m_mutex};
  for (auto&& listener : m_logListeners) {
    if ((flags & listener->eventMask) != 0) {
      int count = 0;
      for (auto&& [finishEvent, mask] : listener->sources) {
        if ((flags & mask) != 0) {
          listener->poller->queue.emplace_back(listener->handle, flags, level,
                                               filename, line, message);
          if (finishEvent &&
              !finishEvent(mask, &listener->poller->queue.back())) {
            listener->poller->queue.pop_back();
          } else {
            ++count;
          }
        }
      }
      if (count > 0) {
        listener->handle.Set();
        listener->poller->handle.Set();
      }
    }
  }
}

void ListenerStorage::NotifyTimeSync(std::span<const NT_Listener> handles,
                                     unsigned int flags,
                                     int64_t serverTimeOffset, int64_t rtt2,
                                     bool valid) {
  if (flags == 0) {
    return;
  }
  std::scoped_lock lock{m_mutex};

  auto doSignal = [&](ListenerData& listener) {
    if ((flags & listener.eventMask) != 0) {
      for (auto&& [finishEvent, mask] : listener.sources) {
        if ((flags & mask) != 0) {
          listener.poller->queue.emplace_back(listener.handle, flags,
                                              serverTimeOffset, rtt2, valid);
          // finishEvent is never set (see InstanceImpl)
        }
      }
      listener.handle.Set();
      listener.poller->handle.Set();
    }
  };

  if (!handles.empty()) {
    for (auto handle : handles) {
      if (auto listener = m_listeners.Get(handle)) {
        doSignal(*listener);
      }
    }
  } else {
    for (auto&& listener : m_timeSyncListeners) {
      doSignal(*listener);
    }
  }
}

NT_Listener ListenerStorage::AddListener(ListenerCallback callback) {
  std::scoped_lock lock{m_mutex};
  if (!m_thread) {
    m_thread.Start(m_pollers.Add(m_inst)->handle);
  }
  if (auto thr = m_thread.GetThread()) {
    auto listener = DoAddListener(thr->m_poller);
    if (listener) {
      thr->m_callbacks.try_emplace(listener, std::move(callback));
    }
    return listener;
  } else {
    return {};
  }
}

NT_Listener ListenerStorage::AddListener(NT_ListenerPoller pollerHandle) {
  std::scoped_lock lock{m_mutex};
  return DoAddListener(pollerHandle);
}

NT_Listener ListenerStorage::DoAddListener(NT_ListenerPoller pollerHandle) {
  if (auto poller = m_pollers.Get(pollerHandle)) {
    return m_listeners.Add(m_inst, poller)->handle;
  } else {
    return {};
  }
}

NT_ListenerPoller ListenerStorage::CreateListenerPoller() {
  std::scoped_lock lock{m_mutex};
  return m_pollers.Add(m_inst)->handle;
}

std::vector<std::pair<NT_Listener, unsigned int>>
ListenerStorage::DestroyListenerPoller(NT_ListenerPoller pollerHandle) {
  std::scoped_lock lock{m_mutex};
  if (auto poller = m_pollers.Remove(pollerHandle)) {
    // ensure all listeners that use this poller are removed
    wpi::SmallVector<NT_Listener, 16> toRemove;
    for (auto&& listener : m_listeners) {
      if (listener->poller == poller.get()) {
        toRemove.emplace_back(listener->handle);
      }
    }
    return DoRemoveListeners(toRemove);
  } else {
    return {};
  }
}

std::vector<Event> ListenerStorage::ReadListenerQueue(
    NT_ListenerPoller pollerHandle) {
  std::scoped_lock lock{m_mutex};
  if (auto poller = m_pollers.Get(pollerHandle)) {
    std::vector<Event> rv;
    rv.swap(poller->queue);
    return rv;
  } else {
    return {};
  }
}

std::vector<std::pair<NT_Listener, unsigned int>>
ListenerStorage::RemoveListener(NT_Listener listenerHandle) {
  std::scoped_lock lock{m_mutex};
  return DoRemoveListeners({&listenerHandle, 1});
}

bool ListenerStorage::WaitForListenerQueue(double timeout) {
  WPI_EventHandle h;
  {
    std::scoped_lock lock{m_mutex};
    if (auto thr = m_thread.GetThread()) {
      h = thr->m_waitQueueWaiter.GetHandle();
      thr->m_waitQueueWakeup.Set();
    } else {
      return false;
    }
  }
  bool timedOut;
  wpi::WaitForObject(h, timeout, &timedOut);
  return !timedOut;
}

void ListenerStorage::Reset() {
  std::scoped_lock lock{m_mutex};
  m_pollers.clear();
  m_listeners.clear();
  m_connListeners.clear();
  m_topicListeners.clear();
  m_valueListeners.clear();
  m_logListeners.clear();
  m_timeSyncListeners.clear();
  if (m_thread) {
    m_thread.Stop();
  }
}

std::vector<std::pair<NT_Listener, unsigned int>>
ListenerStorage::DoRemoveListeners(std::span<const NT_Listener> handles) {
  std::vector<std::pair<NT_Listener, unsigned int>> rv;
  auto thr = m_thread.GetThread();
  for (auto handle : handles) {
    if (auto listener = m_listeners.Remove(handle)) {
      rv.emplace_back(handle, listener->eventMask);
      if (thr) {
        if (thr->m_poller == listener->poller->handle) {
          thr->m_callbacks.erase(handle);
        }
      }
      if ((listener->eventMask & NT_EVENT_CONNECTION) != 0) {
        m_connListeners.Remove(listener.get());
      }
      if ((listener->eventMask & NT_EVENT_TOPIC) != 0) {
        m_topicListeners.Remove(listener.get());
      }
      if ((listener->eventMask & NT_EVENT_VALUE_ALL) != 0) {
        m_valueListeners.Remove(listener.get());
      }
      if ((listener->eventMask & NT_EVENT_LOGMESSAGE) != 0 ||
          (listener->eventMask & 0x1ff0000) != 0) {
        m_logListeners.Remove(listener.get());
      }
      if ((listener->eventMask & NT_EVENT_TIMESYNC) != 0) {
        m_timeSyncListeners.Remove(listener.get());
      }
    }
  }
  return rv;
}
