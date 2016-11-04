/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_NETWORKCONNECTION_H_
#define NT_NETWORKCONNECTION_H_

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>

#include "support/ConcurrentQueue.h"
#include "Message.h"
#include "ntcore_cpp.h"

namespace wpi {
class NetworkStream;
}

namespace nt {

class Notifier;

class NetworkConnection {
 public:
  enum State { kCreated, kInit, kHandshake, kSynchronized, kActive, kDead };

  typedef std::function<bool(
      NetworkConnection& conn,
      std::function<std::shared_ptr<Message>()> get_msg,
      std::function<void(llvm::ArrayRef<std::shared_ptr<Message>>)> send_msgs)>
      HandshakeFunc;
  typedef std::function<void(std::shared_ptr<Message> msg,
                             NetworkConnection* conn)>
      ProcessIncomingFunc;
  typedef std::vector<std::shared_ptr<Message>> Outgoing;
  typedef wpi::ConcurrentQueue<Outgoing> OutgoingQueue;

  NetworkConnection(std::unique_ptr<wpi::NetworkStream> stream,
                    Notifier& notifier, HandshakeFunc handshake,
                    Message::GetEntryTypeFunc get_entry_type);
  ~NetworkConnection();

  // Set the input processor function.  This must be called before Start().
  void set_process_incoming(ProcessIncomingFunc func) {
    m_process_incoming = func;
  }

  void Start();
  void Stop();

  ConnectionInfo info() const;

  bool active() const { return m_active; }
  wpi::NetworkStream& stream() { return *m_stream; }

  void QueueOutgoing(std::shared_ptr<Message> msg);
  void PostOutgoing(bool keep_alive);
  void NotifyIfActive(ConnectionListenerCallback callback) const;

  unsigned int uid() const { return m_uid; }

  unsigned int proto_rev() const { return m_proto_rev; }
  void set_proto_rev(unsigned int proto_rev) { m_proto_rev = proto_rev; }

  State state() const;
  void set_state(State state);

  std::string remote_id() const;
  void set_remote_id(StringRef remote_id);

  unsigned long long last_update() const { return m_last_update; }

  NetworkConnection(const NetworkConnection&) = delete;
  NetworkConnection& operator=(const NetworkConnection&) = delete;

 private:
  void ReadThreadMain();
  void WriteThreadMain();

  static std::atomic_uint s_uid;

  unsigned int m_uid;
  std::unique_ptr<wpi::NetworkStream> m_stream;
  Notifier& m_notifier;
  OutgoingQueue m_outgoing;
  HandshakeFunc m_handshake;
  Message::GetEntryTypeFunc m_get_entry_type;
  ProcessIncomingFunc m_process_incoming;
  std::thread m_read_thread;
  std::thread m_write_thread;
  std::atomic_bool m_active;
  std::atomic_uint m_proto_rev;
  mutable std::mutex m_state_mutex;
  State m_state;
  mutable std::mutex m_remote_id_mutex;
  std::string m_remote_id;
  std::atomic_ullong m_last_update;
  std::chrono::steady_clock::time_point m_last_post;

  std::mutex m_pending_mutex;
  Outgoing m_pending_outgoing;
  std::vector<std::pair<std::size_t, std::size_t>> m_pending_update;

  // Condition variables for shutdown
  std::mutex m_shutdown_mutex;
  std::condition_variable m_read_shutdown_cv;
  std::condition_variable m_write_shutdown_cv;
  bool m_read_shutdown = false;
  bool m_write_shutdown = false;
};

}  // namespace nt

#endif  // NT_NETWORKCONNECTION_H_
