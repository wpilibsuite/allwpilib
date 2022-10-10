// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string_view>
#include <vector>

#include <wpi/mutex.h>

#include "IConnectionList.h"
#include "ntcore_cpp.h"

namespace nt {

class ConnectionList final : public IConnectionList {
 public:
  explicit ConnectionList(int inst);
  ~ConnectionList() final;

  // IConnectionList interface
  int AddConnection(const ConnectionInfo& info) final;
  void RemoveConnection(int handle) final;
  void ClearConnections() final;

  // user-facing functions
  std::vector<ConnectionInfo> GetConnections() const final;
  bool IsConnected() const final;

  NT_ConnectionListener AddListener(
      std::function<void(const ConnectionNotification& event)> callback,
      bool immediateNotify);

  NT_ConnectionListenerPoller CreateListenerPoller();
  void DestroyListenerPoller(NT_ConnectionListenerPoller pollerHandle);
  NT_ConnectionListener AddPolledListener(
      NT_ConnectionListenerPoller pollerHandle, bool immediateNotify);
  std::vector<ConnectionNotification> ReadListenerQueue(
      NT_ConnectionListenerPoller pollerHandle);
  void RemoveListener(NT_ConnectionListener listenerHandle);

  NT_ConnectionDataLogger StartDataLog(wpi::log::DataLog& log,
                                       std::string_view name);
  void StopDataLog(NT_ConnectionDataLogger logger);

 private:
  class Impl;
  std::unique_ptr<Impl> m_impl;

  mutable wpi::mutex m_mutex;
};

}  // namespace nt
