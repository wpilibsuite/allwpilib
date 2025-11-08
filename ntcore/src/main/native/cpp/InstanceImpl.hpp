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

#include "wpi/util/mutex.hpp"

#include "ConnectionList.hpp"
#include "Handle.hpp"
#include "ListenerStorage.hpp"
#include "LocalStorage.hpp"
#include "Log.hpp"
#include "LoggerImpl.hpp"
#include "NetworkClient.hpp"
#include "NetworkServer.hpp"

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
  static constexpr int kNumInstances = 16;
  static std::atomic<InstanceImpl*> s_instances[kNumInstances];
  static wpi::mutex s_mutex;

  struct Cleanup {
    ~Cleanup() {
      for (auto&& inst : s_instances) {
        // don't actually destroy (due to undefined global destruction order),
        // but stop all listener threads
        if (auto i = inst.load()) {
          i->listenerStorage.Reset();
        }
      }
    }
  };
  static Cleanup s_cleanup;

  wpi::mutex m_mutex;
  std::shared_ptr<NetworkServer> m_networkServer;
  std::shared_ptr<INetworkClient> m_networkClient;
  std::vector<std::pair<std::string, unsigned int>> m_servers;
  std::optional<int64_t> m_serverTimeOffset;
  int64_t m_rtt2 = 0;
  int m_inst;
};

}  // namespace nt
