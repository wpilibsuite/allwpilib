// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_DISPATCHER_H_
#define NTCORE_DISPATCHER_H_

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include <wpi/DataLog.h>
#include <wpi/UidVector.h>
#include <wpi/condition_variable.h>
#include <wpi/mutex.h>
#include <wpi/span.h>

#include "IDispatcher.h"
#include "INetworkConnection.h"

namespace wpi {
class Logger;
class NetworkAcceptor;
class NetworkStream;
}  // namespace wpi

namespace nt {

class IConnectionNotifier;
class IStorage;
class NetworkConnection;

class DispatcherBase : public IDispatcher {
  friend class DispatcherTest;

 public:
  using Connector = std::function<std::unique_ptr<wpi::NetworkStream>()>;

  DispatcherBase(IStorage& storage, IConnectionNotifier& notifier,
                 wpi::Logger& logger);
  ~DispatcherBase() override;

  unsigned int GetNetworkMode() const;
  void StartLocal();
  void StartServer(std::string_view persist_filename,
                   std::unique_ptr<wpi::NetworkAcceptor> acceptor);
  void StartClient();
  void Stop();
  void SetUpdateRate(double interval);
  void SetIdentity(std::string_view name);
  void Flush();
  std::vector<ConnectionInfo> GetConnections() const;
  bool IsConnected() const;

  unsigned int AddListener(
      std::function<void(const ConnectionNotification& event)> callback,
      bool immediate_notify) const;
  unsigned int AddPolledListener(unsigned int poller_uid,
                                 bool immediate_notify) const;

  unsigned int StartDataLog(wpi::log::DataLog& log, std::string_view name);
  void StopDataLog(unsigned int logger);

  void SetConnector(Connector connector);
  void SetConnectorOverride(Connector connector);
  void ClearConnectorOverride();

  bool active() const { return m_active; }

  DispatcherBase(const DispatcherBase&) = delete;
  DispatcherBase& operator=(const DispatcherBase&) = delete;

 private:
  void DispatchThreadMain();
  void ServerThreadMain();
  void ClientThreadMain();

  bool ClientHandshake(
      NetworkConnection& conn,
      std::function<std::shared_ptr<Message>()> get_msg,
      std::function<void(wpi::span<std::shared_ptr<Message>>)> send_msgs);
  bool ServerHandshake(
      NetworkConnection& conn,
      std::function<std::shared_ptr<Message>()> get_msg,
      std::function<void(wpi::span<std::shared_ptr<Message>>)> send_msgs);

  void ClientReconnect(unsigned int proto_rev = 0x0300);

  void QueueOutgoing(std::shared_ptr<Message> msg, INetworkConnection* only,
                     INetworkConnection* except) override;

  IStorage& m_storage;
  IConnectionNotifier& m_notifier;
  unsigned int m_networkMode = NT_NET_MODE_NONE;
  std::string m_persist_filename;
  std::thread m_dispatch_thread;
  std::thread m_clientserver_thread;

  std::unique_ptr<wpi::NetworkAcceptor> m_server_acceptor;
  Connector m_client_connector_override;
  Connector m_client_connector;
  uint8_t m_connections_uid = 0;

  // Mutex for user-accessible items
  mutable wpi::mutex m_user_mutex;
  std::vector<std::shared_ptr<INetworkConnection>> m_connections;
  std::string m_identity;

  std::atomic_bool m_active;       // set to false to terminate threads
  std::atomic_uint m_update_rate;  // periodic dispatch update rate, in ms

  // Condition variable for forced dispatch wakeup (flush)
  wpi::mutex m_flush_mutex;
  wpi::condition_variable m_flush_cv;
  uint64_t m_last_flush = 0;
  bool m_do_flush = false;

  // Condition variable for client reconnect (uses user mutex)
  wpi::condition_variable m_reconnect_cv;
  unsigned int m_reconnect_proto_rev = 0x0300;
  bool m_do_reconnect = true;

  struct DataLogger {
    DataLogger() = default;
    DataLogger(wpi::log::DataLog& log, std::string_view name, int64_t time)
        : entry{log, name,
                "{\"schema\":\"NTConnectionInfo\",\"source\":\"NT\"}", "json",
                time} {}

    explicit operator bool() const { return static_cast<bool>(entry); }

    wpi::log::StringLogEntry entry;
    unsigned int notifier = 0;
  };
  wpi::UidVector<DataLogger, 4> m_dataloggers;

 protected:
  wpi::Logger& m_logger;
};

class Dispatcher : public DispatcherBase {
  friend class DispatcherTest;

 public:
  Dispatcher(IStorage& storage, IConnectionNotifier& notifier,
             wpi::Logger& logger)
      : DispatcherBase(storage, notifier, logger) {}

  void StartServer(std::string_view persist_filename,
                   const char* listen_address, unsigned int port);

  void SetServer(const char* server_name, unsigned int port);
  void SetServer(
      wpi::span<const std::pair<std::string_view, unsigned int>> servers);
  void SetServerTeam(unsigned int team, unsigned int port);

  void SetServerOverride(const char* server_name, unsigned int port);
  void ClearServerOverride();
};

}  // namespace nt

#endif  // NTCORE_DISPATCHER_H_
