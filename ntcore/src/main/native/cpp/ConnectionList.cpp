// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ConnectionList.h"

#include <atomic>
#include <optional>

#include <wpi/DataLog.h>
#include <wpi/DenseMap.h>
#include <wpi/SafeThread.h>
#include <wpi/Synchronization.h>
#include <wpi/UidVector.h>
#include <wpi/json_serializer.h>
#include <wpi/raw_ostream.h>

#include "HandleMap.h"
#include "ntcore_cpp.h"

using namespace nt;

namespace {

struct PollerData {
  static constexpr auto kType = Handle::kConnectionListenerPoller;

  explicit PollerData(NT_ConnectionListenerPoller handle) : handle{handle} {}

  wpi::SignalObject<NT_ConnectionListenerPoller> handle;
  std::vector<ConnectionNotification> queue;
};

struct ListenerData {
  static constexpr auto kType = Handle::kConnectionListener;

  ListenerData(NT_ConnectionListener handle, PollerData* poller)
      : handle{handle}, poller{poller} {}

  wpi::SignalObject<NT_ConnectionListener> handle;
  PollerData* poller;
};

struct DataLoggerData {
  static constexpr auto kType = Handle::kConnectionDataLogger;

  DataLoggerData(NT_ConnectionDataLogger handle, wpi::log::DataLog& log,
                 std::string_view name, int64_t time)
      : handle{handle},
        entry{log, name, "{\"schema\":\"NTConnectionInfo\",\"source\":\"NT\"}",
              "json", time} {}

  NT_ConnectionDataLogger handle;
  wpi::log::StringLogEntry entry;
};

class ListenerThread final : public wpi::SafeThreadEvent {
 public:
  explicit ListenerThread(NT_ConnectionListenerPoller poller)
      : m_poller{poller} {}

  void Main() final;

  NT_ConnectionListenerPoller m_poller;
  wpi::DenseMap<NT_ConnectionListener,
                std::function<void(const ConnectionNotification& event)>>
      m_callbacks;
};

class CLImpl {
 public:
  explicit CLImpl(int inst) : m_inst{inst} {}

  int m_inst;

  // shared with user (must be atomic or mutex-protected)
  std::atomic_bool m_connected{false};
  wpi::UidVector<std::optional<ConnectionInfo>, 8> m_connections;

  HandleMap<PollerData, 8> m_pollers;
  HandleMap<ListenerData, 8> m_listeners;
  HandleMap<DataLoggerData, 8> m_dataloggers;

  wpi::SafeThreadOwner<ListenerThread> m_listenerThread;

  NT_ConnectionListener AddListener(
      std::function<void(const ConnectionNotification& event)> callback,
      bool immediateNotify);
  PollerData* CreateListenerPoller() { return m_pollers.Add(m_inst); }
  void DestroyListenerPoller(NT_ConnectionListenerPoller pollerHandle);
  NT_ConnectionListener AddPolledListener(
      NT_ConnectionListenerPoller pollerHandle, bool immediateNotify);
  void RemoveListener(NT_ConnectionListener listenerHandle);
};

}  // namespace

void ListenerThread::Main() {
  while (m_active) {
    WPI_Handle signaledBuf[2];
    auto signaled =
        wpi::WaitForObjects({m_poller, m_stopEvent.GetHandle()}, signaledBuf);
    if (signaled.empty() || !m_active) {
      return;
    }
    // call all the way back out to the C++ API to ensure valid handle
    auto events = nt::ReadConnectionListenerQueue(m_poller);
    if (events.empty()) {
      continue;
    }
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
}

NT_ConnectionListener CLImpl::AddListener(
    std::function<void(const ConnectionNotification& event)> callback,
    bool immediateNotify) {
  if (!m_listenerThread) {
    m_listenerThread.Start(CreateListenerPoller()->handle);
  }
  if (auto thr = m_listenerThread.GetThread()) {
    auto listener = AddPolledListener(thr->m_poller, immediateNotify);
    if (listener) {
      thr->m_callbacks.try_emplace(listener, std::move(callback));
    }
    return listener;
  } else {
    return {};
  }
}

void CLImpl::DestroyListenerPoller(NT_ConnectionListenerPoller pollerHandle) {
  if (auto poller = m_pollers.Remove(pollerHandle)) {
    // ensure all listeners that use this poller are removed
    wpi::SmallVector<NT_ConnectionListener, 16> toRemove;
    for (auto&& listener : m_listeners) {
      if (listener->poller == poller.get()) {
        toRemove.emplace_back(listener->handle);
      }
    }
    for (auto handle : toRemove) {
      RemoveListener(handle);
    }
  }
}

NT_ConnectionListener CLImpl::AddPolledListener(
    NT_ConnectionListenerPoller pollerHandle, bool immediateNotify) {
  auto poller = m_pollers.Get(pollerHandle);
  if (!poller) {
    return {};
  }

  auto listener = m_listeners.Add(m_inst, poller);
  if (immediateNotify && !m_connections.empty()) {
    for (auto&& conn : m_connections) {
      listener->poller->queue.emplace_back(listener->handle.GetHandle(), true,
                                           *conn);
    }
    listener->poller->handle.Set();
    listener->handle.Set();
  }
  return listener->handle;
}

void CLImpl::RemoveListener(NT_ConnectionListener listenerHandle) {
  if (auto listener = m_listeners.Remove(listenerHandle)) {
    if (auto thr = m_listenerThread.GetThread()) {
      if (thr->m_poller == listener->poller->handle) {
        thr->m_callbacks.erase(listenerHandle);
      }
    }
  }
}

static std::string ConnInfoToJson(bool connected, const ConnectionInfo& info) {
  std::string str;
  wpi::raw_string_ostream os{str};
  wpi::json::serializer s{os, ' ', 0};
  os << "{\"connected\":" << (connected ? "true" : "false");
  os << ",\"remote_id\":\"";
  s.dump_escaped(info.remote_id, false);
  os << "\",\"remote_ip\":\"";
  s.dump_escaped(info.remote_ip, false);
  os << "\",\"remote_port\":";
  s.dump_integer(static_cast<uint64_t>(info.remote_port));
  os << ",\"protocol_version\":";
  s.dump_integer(static_cast<uint64_t>(info.protocol_version));
  os << "}";
  os.flush();
  return str;
}

class ConnectionList::Impl : public CLImpl {
 public:
  explicit Impl(int inst) : CLImpl{inst} {}
};

ConnectionList::ConnectionList(int inst)
    : m_impl{std::make_unique<Impl>(inst)} {}

ConnectionList::~ConnectionList() = default;

int ConnectionList::AddConnection(const ConnectionInfo& info) {
  std::scoped_lock lock{m_mutex};
  m_impl->m_connected = true;
  for (auto&& listener : m_impl->m_listeners) {
    listener->poller->queue.emplace_back(listener->handle.GetHandle(), true,
                                         info);
    listener->poller->handle.Set();
    listener->handle.Set();
  }
  if (!m_impl->m_dataloggers.empty()) {
    auto now = Now();
    for (auto&& datalogger : m_impl->m_dataloggers) {
      datalogger->entry.Append(ConnInfoToJson(true, info), now);
    }
  }
  return m_impl->m_connections.emplace_back(info);
}

void ConnectionList::RemoveConnection(int handle) {
  std::scoped_lock lock{m_mutex};
  auto val = m_impl->m_connections.erase(handle);
  if (m_impl->m_connections.empty()) {
    m_impl->m_connected = false;
  }
  if (val) {
    for (auto&& listener : m_impl->m_listeners) {
      listener->poller->queue.emplace_back(listener->handle.GetHandle(), false,
                                           *val);
      listener->poller->handle.Set();
      listener->handle.Set();
    }
    if (!m_impl->m_dataloggers.empty()) {
      auto now = Now();
      for (auto&& datalogger : m_impl->m_dataloggers) {
        datalogger->entry.Append(ConnInfoToJson(false, *val), now);
      }
    }
  }
}

void ConnectionList::ClearConnections() {
  std::scoped_lock lock{m_mutex};
  m_impl->m_connected = false;
  for (auto&& conn : m_impl->m_connections) {
    for (auto&& listener : m_impl->m_listeners) {
      listener->poller->queue.emplace_back(listener->handle.GetHandle(), false,
                                           *conn);
      listener->poller->handle.Set();
      listener->handle.Set();
    }
  }
  m_impl->m_connections.clear();
}

std::vector<ConnectionInfo> ConnectionList::GetConnections() const {
  std::scoped_lock lock{m_mutex};
  std::vector<ConnectionInfo> info;
  info.reserve(m_impl->m_connections.size());
  for (auto&& conn : m_impl->m_connections) {
    info.emplace_back(*conn);
  }
  return info;
}

bool ConnectionList::IsConnected() const {
  return m_impl->m_connected;
}

NT_ConnectionListener ConnectionList::AddListener(
    std::function<void(const ConnectionNotification& event)> callback,
    bool immediateNotify) {
  std::scoped_lock lock{m_mutex};
  return m_impl->AddListener(std::move(callback), immediateNotify);
}

NT_ConnectionListenerPoller ConnectionList::CreateListenerPoller() {
  std::scoped_lock lock{m_mutex};
  return m_impl->CreateListenerPoller()->handle;
}

void ConnectionList::DestroyListenerPoller(
    NT_ConnectionListenerPoller pollerHandle) {
  std::scoped_lock lock{m_mutex};
  m_impl->DestroyListenerPoller(pollerHandle);
}

NT_ConnectionListener ConnectionList::AddPolledListener(
    NT_ConnectionListenerPoller pollerHandle, bool immediateNotify) {
  std::scoped_lock lock{m_mutex};
  return m_impl->AddPolledListener(pollerHandle, immediateNotify);
}

std::vector<ConnectionNotification> ConnectionList::ReadListenerQueue(
    NT_ConnectionListenerPoller pollerHandle) {
  std::scoped_lock lock{m_mutex};
  if (auto poller = m_impl->m_pollers.Get(pollerHandle)) {
    std::vector<ConnectionNotification> rv;
    rv.swap(poller->queue);
    return rv;
  } else {
    return {};
  }
}

void ConnectionList::RemoveListener(NT_ConnectionListener listenerHandle) {
  std::scoped_lock lock{m_mutex};
  m_impl->RemoveListener(listenerHandle);
}

NT_ConnectionDataLogger ConnectionList::StartDataLog(wpi::log::DataLog& log,
                                                     std::string_view name) {
  std::scoped_lock lock{m_mutex};
  auto now = Now();
  auto datalogger = m_impl->m_dataloggers.Add(m_impl->m_inst, log, name, now);
  for (auto&& conn : m_impl->m_connections) {
    datalogger->entry.Append(ConnInfoToJson(true, *conn), now);
  }
  return datalogger->handle;
}

void ConnectionList::StopDataLog(NT_ConnectionDataLogger logger) {
  std::scoped_lock lock{m_mutex};
  if (auto datalogger = m_impl->m_dataloggers.Remove(logger)) {
    datalogger->entry.Finish(Now());
  }
}
