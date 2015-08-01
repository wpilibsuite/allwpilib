/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_NETWORKCONNECTION_H_
#define NT_NETWORKCONNECTION_H_

#include <atomic>
#include <memory>
#include <thread>

#include "support/ConcurrentQueue.h"
#include "Message.h"

class TCPStream;

namespace nt {

class NetworkConnection {
 public:
  enum State { kCreated, kInit, kHandshake, kActive, kDead };

  typedef std::function<bool(
      NetworkConnection& conn,
      std::function<std::shared_ptr<Message>()> get_msg,
      std::function<void(llvm::ArrayRef<std::shared_ptr<Message>>)> send_msgs)>
      HandshakeFunc;
  typedef std::function<void(std::shared_ptr<Message> msg,
                             NetworkConnection* conn, unsigned int proto_rev)>
      ProcessIncomingFunc;
  typedef std::vector<std::shared_ptr<Message>> Outgoing;
  typedef ConcurrentQueue<Outgoing> OutgoingQueue;

  NetworkConnection(std::unique_ptr<TCPStream> stream,
                    HandshakeFunc handshake,
                    Message::GetEntryTypeFunc get_entry_type,
                    ProcessIncomingFunc process_incoming);
  ~NetworkConnection();

  void Start();
  void Stop();

  bool active() const { return m_active; }
  TCPStream& stream() { return *m_stream; }
  OutgoingQueue& outgoing() { return m_outgoing; }

  unsigned int proto_rev() const { return m_proto_rev; }
  void set_proto_rev(unsigned int proto_rev) { m_proto_rev = proto_rev; }

  State state() const { return static_cast<State>(m_state.load()); }
  void set_state(State state) { m_state = static_cast<int>(state); }

  std::string remote_id() const;
  void set_remote_id(StringRef remote_id);

  NetworkConnection(const NetworkConnection&) = delete;
  NetworkConnection& operator=(const NetworkConnection&) = delete;

 private:
  void ReadThreadMain();
  void WriteThreadMain();

  std::unique_ptr<TCPStream> m_stream;
  OutgoingQueue m_outgoing;
  HandshakeFunc m_handshake;
  Message::GetEntryTypeFunc m_get_entry_type;
  ProcessIncomingFunc m_process_incoming;
  std::thread m_read_thread;
  std::thread m_write_thread;
  std::atomic_bool m_active;
  std::atomic_uint m_proto_rev;
  std::atomic_int m_state;
  mutable std::mutex m_remote_id_mutex;
  std::string m_remote_id;
};

}  // namespace nt

#endif  // NT_NETWORKCONNECTION_H_
