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

#include <wpinet/EventLoopRunner.h>
#include <wpinet/uv/Async.h>
#include <wpinet/uv/Idle.h>
#include <wpinet/uv/Timer.h>

#include "net/ClientMessageQueue.h"
#include "net/Message.h"
#include "ntcore_cpp.h"
#include "server/ServerImpl.h"

namespace wpi {
class Logger;
}  // namespace wpi

namespace nt::net {
class ILocalStorage;
}  // namespace nt::net

namespace nt {

class IConnectionList;

class NetworkServer {
 public:
  NetworkServer(std::string_view persistentFilename,
                std::string_view listenAddress, unsigned int port3,
                unsigned int port4, net::ILocalStorage& localStorage,
                IConnectionList& connList, wpi::Logger& logger,
                std::function<void()> initDone);
  ~NetworkServer();

  void FlushLocal();
  void Flush();

 private:
  class ServerConnection;
  class ServerConnection3;
  class ServerConnection4;

  void ProcessAllLocal();
  void LoadPersistent();
  void SavePersistent(std::string_view filename, std::string_view data);
  void Init();
  void AddConnection(ServerConnection* conn, const ConnectionInfo& info);
  void RemoveConnection(ServerConnection* conn);

  net::ILocalStorage& m_localStorage;
  IConnectionList& m_connList;
  wpi::Logger& m_logger;
  std::function<void()> m_initDone;
  std::string m_persistentData;
  std::string m_persistentFilename;
  std::string m_listenAddress;
  unsigned int m_port3;
  unsigned int m_port4;

  // used only from loop
  std::shared_ptr<wpi::uv::Timer> m_readLocalTimer;
  std::shared_ptr<wpi::uv::Timer> m_savePersistentTimer;
  std::shared_ptr<wpi::uv::Async<>> m_flushLocal;
  std::shared_ptr<wpi::uv::Async<>> m_flush;
  std::shared_ptr<wpi::uv::Idle> m_idle;
  bool m_shutdown = false;

  using Queue = net::LocalClientMessageQueue;
  net::ClientMessage m_localMsgs[Queue::kBlockSize];

  server::ServerImpl m_serverImpl;

  // shared with user (must be atomic or mutex-protected)
  std::atomic<wpi::uv::Async<>*> m_flushLocalAtomic{nullptr};
  std::atomic<wpi::uv::Async<>*> m_flushAtomic{nullptr};
  mutable wpi::mutex m_mutex;
  struct Connection {
    ServerConnection* conn;
    int connHandle;
  };
  std::vector<Connection> m_connections;

  Queue m_localQueue;

  wpi::EventLoopRunner m_loopRunner;
  wpi::uv::Loop& m_loop;
};

}  // namespace nt
