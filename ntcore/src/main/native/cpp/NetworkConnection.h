/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_NETWORKCONNECTION_H_
#define NTCORE_NETWORKCONNECTION_H_

#include <stdint.h>

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <wpi/ConcurrentQueue.h>
#include <wpi/condition_variable.h>
#include <wpi/mutex.h>

#include "INetworkConnection.h"
#include "Message.h"
#include "ntcore_cpp.h"

namespace wpi {
class Logger;
class NetworkStream;
}  // namespace wpi

namespace nt {

class IConnectionNotifier;

class NetworkConnection : public INetworkConnection {
 public:
  typedef std::function<bool(
      NetworkConnection& conn,
      std::function<std::shared_ptr<Message>()> get_msg,
      std::function<void(wpi::ArrayRef<std::shared_ptr<Message>>)> send_msgs)>
      HandshakeFunc;
  typedef std::function<void(std::shared_ptr<Message> msg,
                             NetworkConnection* conn)>
      ProcessIncomingFunc;
  typedef std::vector<std::shared_ptr<Message>> Outgoing;
  typedef wpi::ConcurrentQueue<Outgoing> OutgoingQueue;

  NetworkConnection(unsigned int uid,
                    std::unique_ptr<wpi::NetworkStream> stream,
                    IConnectionNotifier& notifier, wpi::Logger& logger,
                    HandshakeFunc handshake,
                    Message::GetEntryTypeFunc get_entry_type);
  ~NetworkConnection();

  // Set the input processor function.  This must be called before Start().
  void set_process_incoming(ProcessIncomingFunc func) {
    m_process_incoming = func;
  }

  void Start();
  void Stop();

  ConnectionInfo info() const override;

  bool active() const { return m_active; }
  wpi::NetworkStream& stream() { return *m_stream; }

  void QueueOutgoing(std::shared_ptr<Message> msg) override;
  void PostOutgoing(bool keep_alive) override;

  unsigned int uid() const { return m_uid; }

  unsigned int proto_rev() const override;
  void set_proto_rev(unsigned int proto_rev) override;

  State state() const override;
  void set_state(State state) override;

  std::string remote_id() const;
  void set_remote_id(StringRef remote_id);

  uint64_t last_update() const { return m_last_update; }

  NetworkConnection(const NetworkConnection&) = delete;
  NetworkConnection& operator=(const NetworkConnection&) = delete;

 private:
  void ReadThreadMain();
  void WriteThreadMain();

  unsigned int m_uid;
  std::unique_ptr<wpi::NetworkStream> m_stream;
  IConnectionNotifier& m_notifier;
  wpi::Logger& m_logger;
  OutgoingQueue m_outgoing;
  HandshakeFunc m_handshake;
  Message::GetEntryTypeFunc m_get_entry_type;
  ProcessIncomingFunc m_process_incoming;
  std::thread m_read_thread;
  std::thread m_write_thread;
  std::atomic_bool m_active;
  std::atomic_uint m_proto_rev;
  mutable wpi::mutex m_state_mutex;
  State m_state;
  mutable wpi::mutex m_remote_id_mutex;
  std::string m_remote_id;
  std::atomic_ullong m_last_update;
  std::chrono::steady_clock::time_point m_last_post;

  wpi::mutex m_pending_mutex;
  Outgoing m_pending_outgoing;
  std::vector<std::pair<size_t, size_t>> m_pending_update;

  // Condition variables for shutdown
  wpi::mutex m_shutdown_mutex;
  wpi::condition_variable m_read_shutdown_cv;
  wpi::condition_variable m_write_shutdown_cv;
  bool m_read_shutdown = false;
  bool m_write_shutdown = false;
};

}  // namespace nt

#endif  // NTCORE_NETWORKCONNECTION_H_
