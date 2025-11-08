// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "INetworkClient.hpp"
#include "net/ClientImpl.hpp"
#include "net/ClientMessageQueue.hpp"
#include "net/Message.hpp"
#include "net/WebSocketConnection.hpp"
#include "wpi/net/DsClient.hpp"
#include "wpi/net/EventLoopRunner.hpp"
#include "wpi/net/ParallelTcpConnector.hpp"
#include "wpi/net/WebSocket.hpp"
#include "wpi/net/uv/Async.hpp"
#include "wpi/net/uv/Timer.hpp"

namespace wpi::util {
class Logger;
}  // namespace wpi

namespace wpi::nt::net {
class ILocalStorage;
}  // namespace wpi::nt::net

namespace wpi::nt {

class IConnectionList;

class NetworkClientBase : public INetworkClient {
 public:
  NetworkClientBase(int inst, std::string_view id,
                    net::ILocalStorage& localStorage, IConnectionList& connList,
                    wpi::util::Logger& logger);
  ~NetworkClientBase() override;

  void Disconnect() override;

  void StartDSClient(unsigned int port) override;
  void StopDSClient() override;

  void FlushLocal() override;
  void Flush() override;

 protected:
  void DoSetServers(
      std::span<const std::pair<std::string, unsigned int>> servers,
      unsigned int defaultPort);

  virtual void TcpConnected(wpi::net::uv::Tcp& tcp) = 0;
  virtual void ForceDisconnect(std::string_view reason) = 0;
  virtual void DoDisconnect(std::string_view reason);

  // invariants
  int m_inst;
  net::ILocalStorage& m_localStorage;
  IConnectionList& m_connList;
  wpi::util::Logger& m_logger;
  std::string m_id;

  // used only from loop
  std::shared_ptr<wpi::net::ParallelTcpConnector> m_parallelConnect;
  std::shared_ptr<wpi::net::uv::Timer> m_readLocalTimer;
  std::shared_ptr<wpi::net::uv::Timer> m_sendOutgoingTimer;
  std::shared_ptr<wpi::net::uv::Async<>> m_flushLocal;
  std::shared_ptr<wpi::net::uv::Async<>> m_flush;

  using Queue = net::LocalClientMessageQueue;
  net::ClientMessage m_localMsgs[Queue::kBlockSize];

  std::vector<std::pair<std::string, unsigned int>> m_servers;

  std::pair<std::string, unsigned int> m_dsClientServer{"", 0};
  std::shared_ptr<wpi::net::DsClient> m_dsClient;

  // shared with user
  std::atomic<wpi::net::uv::Async<>*> m_flushLocalAtomic{nullptr};
  std::atomic<wpi::net::uv::Async<>*> m_flushAtomic{nullptr};

  Queue m_localQueue;

  int m_connHandle = 0;

  wpi::net::EventLoopRunner m_loopRunner;
  wpi::net::uv::Loop& m_loop;
};

class NetworkClient final : public NetworkClientBase {
 public:
  NetworkClient(
      int inst, std::string_view id, net::ILocalStorage& localStorage,
      IConnectionList& connList, wpi::util::Logger& logger,
      std::function<void(int64_t serverTimeOffset, int64_t rtt2, bool valid)>
          timeSyncUpdated);
  ~NetworkClient() final;

  void SetServers(
      std::span<const std::pair<std::string, unsigned int>> servers) final {
    DoSetServers(servers, NT_DEFAULT_PORT);
  }

 private:
  void HandleLocal();
  void TcpConnected(wpi::net::uv::Tcp& tcp) final;
  void WsConnected(wpi::net::WebSocket& ws, wpi::net::uv::Tcp& tcp,
                   std::string_view protocol);
  void ForceDisconnect(std::string_view reason) override;
  void DoDisconnect(std::string_view reason) override;

  std::function<void(int64_t serverTimeOffset, int64_t rtt2, bool valid)>
      m_timeSyncUpdated;
  std::shared_ptr<net::WebSocketConnection> m_wire;
  std::unique_ptr<net::ClientImpl> m_clientImpl;
};

}  // namespace wpi::nt
