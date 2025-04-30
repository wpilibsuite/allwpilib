// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <wpi/mutex.h>

#include "ConnectionList.h"
#include "Handle.h"
#include "ListenerStorage.h"
#include "LocalStorage.h"
#include "Log.h"
#include "LoggerImpl.h"
#include "NetworkClient.h"
#include "NetworkServer.h"

namespace nt {

class InstanceImpl {
 public:
  explicit InstanceImpl(int inst);

  // Instance repository
  static InstanceImpl* GetDefault();
  static InstanceImpl* Get(int inst);
  static InstanceImpl* GetHandle(NT_Handle handle) {
    return Get(Handle{handle}.GetInst());
  }
  static InstanceImpl* GetTyped(NT_Handle handle, Handle::Type type) {
    return Get(Handle{handle}.GetTypedInst(type));
  }
  static int GetDefaultIndex();
  static int Alloc();
  static void Destroy(int inst);

  void StartLocal();
  void StopLocal();
  void StartServer(std::string_view persistFilename,
                   std::string_view listenAddress, unsigned int port);
  void StopServer();
  void StartClient(std::string_view identity);
  void StopClient();
  void SetServers(
      std::span<const std::pair<std::string, unsigned int>> servers);

  std::shared_ptr<NetworkServer> GetServer();
  std::shared_ptr<INetworkClient> GetClient();

  std::optional<int64_t> GetServerTimeOffset();
  void AddTimeSyncListener(NT_Listener listener, unsigned int eventMask);

  void Reset();

  ListenerStorage listenerStorage;
  LoggerImpl logger_impl;
  wpi::Logger logger;
  ConnectionList connectionList;
  LocalStorage localStorage;
  std::atomic<int> networkMode{NT_NET_MODE_NONE};

 private:
  static int AllocImpl();

  static std::atomic<int> s_default;
  static constexpr int NUM_INSTANCES = 16;
  static std::atomic<InstanceImpl*> s_instances[NUM_INSTANCES];
  static wpi::mutex s_mutex;

  wpi::mutex m_mutex;
  std::shared_ptr<NetworkServer> m_networkServer;
  std::shared_ptr<INetworkClient> m_networkClient;
  std::vector<std::pair<std::string, unsigned int>> m_servers;
  std::optional<int64_t> m_serverTimeOffset;
  int64_t m_rtt2 = 0;
  int m_inst;
};

}  // namespace nt
