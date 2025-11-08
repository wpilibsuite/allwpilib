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

#include "wpi/net/DsClient.hpp"
#include "wpi/net/EventLoopRunner.hpp"
#include "wpi/net/ParallelTcpConnector.hpp"
#include "wpi/net/WebSocket.hpp"
#include "wpi/net/uv/Async.hpp"
#include "wpi/net/uv/Timer.hpp"

#include "INetworkClient.hpp"
#include "net/ClientImpl.hpp"
#include "net/ClientMessageQueue.hpp"
#include "net/Message.hpp"
#include "net/WebSocketConnection.hpp"

namespace wpi {
class Logger;
}  // namespace wpi

namespace nt::net {
class ILocalStorage;
}  // namespace nt::net

namespace nt {

class IConnectionList;

class NetworkClientBase : public INetworkClient {
 public:
  NetworkClientBase(int inst, std::string_view id,
                    net::ILocalStorage& localStorage, IConnectionList& connList,
                    wpi::Logger& logger);
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

  virtual void TcpConnected(wpi::uv::Tcp& tcp) = 0;
  virtual void ForceDisconnect(std::string_view reason) = 0;
  virtual void DoDisconnect(std::string_view reason);

  // invariants
  int m_inst;
  net::ILocalStorage& m_localStorage;
  IConnectionList& m_connList;
  wpi::Logger& m_logger;
  std::string m_id;

  // used only from loop
  std::shared_ptr<wpi::ParallelTcpConnector> m_parallelConnect;
  std::shared_ptr<wpi::uv::Timer> m_readLocalTimer;
  std::shared_ptr<wpi::uv::Timer> m_sendOutgoingTimer;
  std::shared_ptr<wpi::uv::Async<>> m_flushLocal;
  std::shared_ptr<wpi::uv::Async<>> m_flush;

  using Queue = net::LocalClientMessageQueue;
  net::ClientMessage m_localMsgs[Queue::kBlockSize];

  std::vector<std::pair<std::string, unsigned int>> m_servers;

  std::pair<std::string, unsigned int> m_dsClientServer{"", 0};
  std::shared_ptr<wpi::DsClient> m_dsClient;

  // shared with user
  std::atomic<wpi::uv::Async<>*> m_flushLocalAtomic{nullptr};
  std::atomic<wpi::uv::Async<>*> m_flushAtomic{nullptr};

  Queue m_localQueue;

  int m_connHandle = 0;

  wpi::EventLoopRunner m_loopRunner;
  wpi::uv::Loop& m_loop;
};

class NetworkClient final : public NetworkClientBase {
 public:
  NetworkClient(
      int inst, std::string_view id, net::ILocalStorage& localStorage,
      IConnectionList& connList, wpi::Logger& logger,
      std::function<void(int64_t serverTimeOffset, int64_t rtt2, bool valid)>
          timeSyncUpdated);
  ~NetworkClient() final;

  void SetServers(
      std::span<const std::pair<std::string, unsigned int>> servers) final {
    DoSetServers(servers, NT_DEFAULT_PORT);
  }

 private:
  void HandleLocal();
  void TcpConnected(wpi::uv::Tcp& tcp) final;
  void WsConnected(wpi::WebSocket& ws, wpi::uv::Tcp& tcp,
                   std::string_view protocol);
  void ForceDisconnect(std::string_view reason) override;
  void DoDisconnect(std::string_view reason) override;

  std::function<void(int64_t serverTimeOffset, int64_t rtt2, bool valid)>
      m_timeSyncUpdated;
  std::shared_ptr<net::WebSocketConnection> m_wire;
  std::unique_ptr<net::ClientImpl> m_clientImpl;
};

}  // namespace nt
