// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "net/ClientMessageQueue.hpp"
#include "net/Message.hpp"
#include "server/ServerImpl.hpp"
#include "wpi/net/EventLoopRunner.hpp"
#include "wpi/net/uv/Async.hpp"
#include "wpi/net/uv/Idle.hpp"
#include "wpi/net/uv/Timer.hpp"
#include "wpi/nt/ntcore_cpp.hpp"

namespace wpi::util {
class Logger;
}  // namespace wpi::util

namespace wpi::nt::net {
class ILocalStorage;
}  // namespace wpi::nt::net

namespace wpi::nt {

class IConnectionList;

class NetworkServer {
 public:
  NetworkServer(std::string_view persistentFilename,
                std::string_view listenAddress, unsigned int port,
                net::ILocalStorage& localStorage, IConnectionList& connList,
                wpi::util::Logger& logger, std::function<void()> initDone);
  ~NetworkServer();

  void FlushLocal();
  void Flush();

 private:
  class ServerConnection;
  class ServerConnection4;

  void ProcessAllLocal();
  void LoadPersistent();
  void SavePersistent(std::string_view filename, std::string_view data);
  void Init();
  void AddConnection(ServerConnection* conn, const ConnectionInfo& info);
  void RemoveConnection(ServerConnection* conn);

  net::ILocalStorage& m_localStorage;
  IConnectionList& m_connList;
  wpi::util::Logger& m_logger;
  std::function<void()> m_initDone;
  std::string m_persistentData;
  std::string m_persistentFilename;
  std::string m_listenAddress;
  unsigned int m_port;

  // used only from loop
  std::shared_ptr<wpi::net::uv::Timer> m_readLocalTimer;
  std::shared_ptr<wpi::net::uv::Timer> m_savePersistentTimer;
  std::shared_ptr<wpi::net::uv::Async<>> m_flushLocal;
  std::shared_ptr<wpi::net::uv::Async<>> m_flush;
  std::shared_ptr<wpi::net::uv::Idle> m_idle;
  bool m_shutdown = false;

  using Queue = net::LocalClientMessageQueue;
  net::ClientMessage m_localMsgs[Queue::kBlockSize];

  server::ServerImpl m_serverImpl;

  // shared with user (must be atomic or mutex-protected)
  std::atomic<wpi::net::uv::Async<>*> m_flushLocalAtomic{nullptr};
  std::atomic<wpi::net::uv::Async<>*> m_flushAtomic{nullptr};
  mutable wpi::util::mutex m_mutex;
  struct Connection {
    ServerConnection* conn;
    int connHandle;
  };
  std::vector<Connection> m_connections;

  Queue m_localQueue;

  wpi::net::EventLoopRunner m_loopRunner;
  wpi::net::uv::Loop& m_loop;
};

}  // namespace wpi::nt
