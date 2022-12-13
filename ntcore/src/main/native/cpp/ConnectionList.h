// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <atomic>
#include <functional>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

#include <wpi/DataLog.h>
#include <wpi/UidVector.h>
#include <wpi/mutex.h>

#include "Handle.h"
#include "HandleMap.h"
#include "IConnectionList.h"
#include "ntcore_cpp.h"

namespace nt {

class IListenerStorage;

class ConnectionList final : public IConnectionList {
 public:
  ConnectionList(int inst, IListenerStorage& listenerStorage);
  ~ConnectionList() final;

  // IConnectionList interface
  int AddConnection(const ConnectionInfo& info) final;
  void RemoveConnection(int handle) final;
  void ClearConnections() final;

  // user-facing functions
  std::vector<ConnectionInfo> GetConnections() const final;
  bool IsConnected() const final;

  void AddListener(NT_Listener listener, unsigned int eventMask);

  NT_ConnectionDataLogger StartDataLog(wpi::log::DataLog& log,
                                       std::string_view name);
  void StopDataLog(NT_ConnectionDataLogger logger);

 private:
  int m_inst;
  IListenerStorage& m_listenerStorage;
  mutable wpi::mutex m_mutex;

  // shared with user (must be atomic or mutex-protected)
  std::atomic_bool m_connected{false};
  wpi::UidVector<std::optional<ConnectionInfo>, 8> m_connections;

  struct DataLoggerData {
    static constexpr auto kType = Handle::kConnectionDataLogger;

    DataLoggerData(NT_ConnectionDataLogger handle, wpi::log::DataLog& log,
                   std::string_view name, int64_t time)
        : handle{handle},
          entry{log, name,
                "{\"schema\":\"NTConnectionInfo\",\"source\":\"NT\"}", "json",
                time} {}

    NT_ConnectionDataLogger handle;
    wpi::log::StringLogEntry entry;
  };
  HandleMap<DataLoggerData, 8> m_dataloggers;
};

}  // namespace nt
