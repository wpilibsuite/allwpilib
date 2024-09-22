// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ConnectionList.h"

#include <string>
#include <vector>

#include <wpi/SmallVector.h>
#include <wpi/json.h>
#include <wpi/raw_ostream.h>

#include "IListenerStorage.h"
#include "ntcore_c.h"
#include "ntcore_cpp.h"

using namespace nt;

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

ConnectionList::ConnectionList(int inst, IListenerStorage& listenerStorage)
    : m_inst{inst}, m_listenerStorage{listenerStorage} {}

ConnectionList::~ConnectionList() = default;

int ConnectionList::AddConnection(const ConnectionInfo& info) {
  std::scoped_lock lock{m_mutex};
  m_connected = true;
  m_listenerStorage.Notify({}, NT_EVENT_CONNECTED, &info);
  if (!m_dataloggers.empty()) {
    auto now = Now();
    for (auto&& datalogger : m_dataloggers) {
      datalogger->entry.Append(ConnInfoToJson(true, info), now);
    }
  }
  return m_connections.emplace_back(info);
}

void ConnectionList::RemoveConnection(int handle) {
  std::scoped_lock lock{m_mutex};
  auto val = m_connections.erase(handle);
  if (m_connections.empty()) {
    m_connected = false;
  }
  if (val) {
    m_listenerStorage.Notify({}, NT_EVENT_DISCONNECTED, &(*val));
    if (!m_dataloggers.empty()) {
      auto now = Now();
      for (auto&& datalogger : m_dataloggers) {
        datalogger->entry.Append(ConnInfoToJson(false, *val), now);
      }
    }
  }
}

void ConnectionList::ClearConnections() {
  std::scoped_lock lock{m_mutex};
  m_connected = false;
  for (auto&& conn : m_connections) {
    m_listenerStorage.Notify({}, NT_EVENT_DISCONNECTED, &(*conn));
  }
  m_connections.clear();
}

std::vector<ConnectionInfo> ConnectionList::GetConnections() const {
  std::scoped_lock lock{m_mutex};
  std::vector<ConnectionInfo> info;
  info.reserve(m_connections.size());
  for (auto&& conn : m_connections) {
    info.emplace_back(*conn);
  }
  return info;
}

bool ConnectionList::IsConnected() const {
  return m_connected;
}

void ConnectionList::AddListener(NT_Listener listener, unsigned int eventMask) {
  std::scoped_lock lock{m_mutex};
  eventMask &= (NT_EVENT_CONNECTION | NT_EVENT_IMMEDIATE);
  m_listenerStorage.Activate(listener, eventMask);
  if ((eventMask & (NT_EVENT_CONNECTED | NT_EVENT_IMMEDIATE)) ==
          (NT_EVENT_CONNECTED | NT_EVENT_IMMEDIATE) &&
      !m_connections.empty()) {
    wpi::SmallVector<const ConnectionInfo*, 16> infos;
    infos.reserve(m_connections.size());
    for (auto&& conn : m_connections) {
      infos.emplace_back(&(*conn));
    }
    m_listenerStorage.Notify({&listener, 1},
                             NT_EVENT_CONNECTED | NT_EVENT_IMMEDIATE, infos);
  }
}

NT_ConnectionDataLogger ConnectionList::StartDataLog(wpi::log::DataLog& log,
                                                     std::string_view name) {
  std::scoped_lock lock{m_mutex};
  auto now = Now();
  auto datalogger = m_dataloggers.Add(m_inst, log, name, now);
  for (auto&& conn : m_connections) {
    datalogger->entry.Append(ConnInfoToJson(true, *conn), now);
  }
  return datalogger->handle;
}

void ConnectionList::StopDataLog(NT_ConnectionDataLogger logger) {
  std::scoped_lock lock{m_mutex};
  if (auto datalogger = m_dataloggers.Remove(logger)) {
    datalogger->entry.Finish(Now());
  }
}
