/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_DISPATCHER_H_
#define NT_DISPATCHER_H_

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "llvm/StringRef.h"

#include "NetworkConnection.h"

namespace nt {

class Dispatcher {
 public:
  static Dispatcher& GetInstance() {
    if (!m_instance) m_instance.reset(new Dispatcher);
    return *m_instance;
  }
  ~Dispatcher();

  void StartServer(const char* listen_address, unsigned int port);
  void StartClient(const char* server_name, unsigned int port);
  void Stop();
  void SetUpdateRate(double interval);
  void SetIdentity(llvm::StringRef name);

  Dispatcher(const Dispatcher&) = delete;
  Dispatcher& operator=(const Dispatcher&) = delete;

 private:
  Dispatcher();

  void DispatchThreadMain();
  void ServerThreadMain(const char* listen_address, unsigned int port);
  void ClientThreadMain(const char* server_name, unsigned int port);

  struct Connection {
    enum State {
    };
    State state;
    std::unique_ptr<NetworkConnection> conn;
  };
  std::thread m_dispatch_thread;
  std::thread m_clientserver_thread;

  // Mutex protected
  std::vector<Connection> m_connections;
  std::string m_identity;
  std::mutex m_user_mutex;

  std::atomic_bool m_active;
  std::atomic_uint m_interval;

  static std::unique_ptr<Dispatcher> m_instance;
};

}  // namespace nt

#endif  // NT_DISPATCHER_H_
