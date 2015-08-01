/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_DISPATCHER_H_
#define NT_DISPATCHER_H_

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "llvm/StringRef.h"

#include "atomic_static.h"
#include "NetworkConnection.h"
#include "Storage.h"

class TCPAcceptor;

namespace nt {

class Dispatcher {
 public:
  static Dispatcher& GetInstance() {
    ATOMIC_STATIC(Dispatcher, instance);
    return instance;
  }
  ~Dispatcher();

  void StartServer(const char* listen_address, unsigned int port);
  void StartClient(const char* server_name, unsigned int port);
  void Stop();
  void SetUpdateRate(double interval);
  void SetIdentity(llvm::StringRef name);
  void Flush();

  bool active() const { return m_active; }

  Dispatcher(const Dispatcher&) = delete;
  Dispatcher& operator=(const Dispatcher&) = delete;

 private:
  Dispatcher();

  void DispatchThreadMain();
  void ServerThreadMain(const char* listen_address, unsigned int port);
  void ClientThreadMain(const char* server_name, unsigned int port);

  bool ClientHandshake(
      NetworkConnection& conn,
      std::function<std::shared_ptr<Message>()> get_msg,
      std::function<void(llvm::ArrayRef<std::shared_ptr<Message>>)> send_msgs);
  bool ServerHandshake(
      NetworkConnection& conn,
      std::function<std::shared_ptr<Message>()> get_msg,
      std::function<void(llvm::ArrayRef<std::shared_ptr<Message>>)> send_msgs);

  void ClientReconnect(unsigned int proto_rev = 0x0300);

  void QueueOutgoing(std::shared_ptr<Message> msg, NetworkConnection* only,
                     NetworkConnection* except);

  bool m_server;
  std::thread m_dispatch_thread;
  std::thread m_clientserver_thread;
  std::thread m_notifier_thread;

  std::shared_ptr<TCPAcceptor> m_server_acceptor;

  // Mutex for user-accessible items
  std::mutex m_user_mutex;
  struct Connection {
    Connection() = default;
    explicit Connection(std::unique_ptr<NetworkConnection> net_)
        : net(std::move(net_)) {}
    std::unique_ptr<NetworkConnection> net;
    NetworkConnection::Outgoing outgoing;
  };
  std::vector<Connection> m_connections;
  std::string m_identity;

  std::atomic_bool m_active;  // set to false to terminate threads
  std::atomic_uint m_update_rate;  // periodic dispatch update rate, in ms

  // Condition variable for forced dispatch wakeup (flush)
  std::mutex m_flush_mutex;
  std::condition_variable m_flush_cv;
  std::chrono::steady_clock::time_point m_last_flush;
  bool m_do_flush;

  // Condition variable for client reconnect
  std::mutex m_reconnect_mutex;
  std::condition_variable m_reconnect_cv;
  unsigned int m_reconnect_proto_rev;
  bool m_do_reconnect;

  ATOMIC_STATIC_DECL(Dispatcher)
};

}  // namespace nt

#endif  // NT_DISPATCHER_H_
