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
  typedef std::shared_ptr<Message> Incoming;
  typedef ConcurrentQueue<Incoming> IncomingQueue;
  typedef std::vector<std::shared_ptr<Message>> Outgoing;
  typedef ConcurrentQueue<Outgoing> OutgoingQueue;

  NetworkConnection(std::unique_ptr<TCPStream> stream,
                    Message::GetEntryTypeFunc get_entry_type);
  ~NetworkConnection();

  void Start();
  void Stop();

  bool active() const { return m_active; }
  TCPStream& stream() { return *m_stream; }
  OutgoingQueue& outgoing() { return m_outgoing; }
  IncomingQueue& incoming() { return m_incoming; }
  void set_proto_rev(unsigned int proto_rev) { m_proto_rev = proto_rev; }

  NetworkConnection(const NetworkConnection&) = delete;
  NetworkConnection& operator=(const NetworkConnection&) = delete;

 private:
  void ReadThreadMain();
  void WriteThreadMain();

  std::unique_ptr<TCPStream> m_stream;
  OutgoingQueue m_outgoing;
  IncomingQueue m_incoming;
  Message::GetEntryTypeFunc m_get_entry_type;
  std::thread m_read_thread;
  std::thread m_write_thread;
  std::atomic_bool m_active;
  std::atomic_uint m_proto_rev;
};

}  // namespace nt

#endif  // NT_NETWORKCONNECTION_H_
