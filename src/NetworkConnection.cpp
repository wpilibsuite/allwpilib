/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NetworkConnection.h"

#include "tcpsockets/TCPStream.h"
#include "Log.h"
#include "raw_socket_istream.h"
#include "WireDecoder.h"
#include "WireEncoder.h"

using namespace nt;

NetworkConnection::NetworkConnection(std::unique_ptr<TCPStream> stream,
                                     HandshakeFunc handshake,
                                     Message::GetEntryTypeFunc get_entry_type,
                                     ProcessIncomingFunc process_incoming)
    : m_stream(std::move(stream)),
      m_handshake(handshake),
      m_get_entry_type(get_entry_type),
      m_process_incoming(process_incoming) {
  m_active = false;
  m_proto_rev = 0x0300;
  m_state = static_cast<int>(kCreated);
}

NetworkConnection::~NetworkConnection() { Stop(); }

void NetworkConnection::Start() {
  if (m_active) return;
  m_active = true;
  m_state = static_cast<int>(kInit);
  // clear queue
  while (!m_outgoing.empty()) m_outgoing.pop();
  // start threads
  m_write_thread = std::thread(&NetworkConnection::WriteThreadMain, this);
  m_read_thread = std::thread(&NetworkConnection::ReadThreadMain, this);
}

void NetworkConnection::Stop() {
  m_state = static_cast<int>(kDead);
  m_active = false;
  // closing the stream so the read thread terminates
  if (m_stream) m_stream->close();
  // send an empty outgoing message set so the write thread terminates
  m_outgoing.push(Outgoing());
  // wait for threads to terminate
  if (m_write_thread.joinable()) m_write_thread.join();
  if (m_read_thread.joinable()) m_read_thread.join();
  // clear queue
  while (!m_outgoing.empty()) m_outgoing.pop();
}

std::string NetworkConnection::remote_id() const {
  std::lock_guard<std::mutex> lock(m_remote_id_mutex);
  return m_remote_id;
}

void NetworkConnection::set_remote_id(StringRef remote_id) {
  std::lock_guard<std::mutex> lock(m_remote_id_mutex);
  m_remote_id = remote_id;
}

void NetworkConnection::ReadThreadMain() {
  raw_socket_istream is(*m_stream);
  WireDecoder decoder(is, m_proto_rev);

  m_state = static_cast<int>(kHandshake);
  if (!m_handshake(*this,
                   [&] {
                     decoder.set_proto_rev(m_proto_rev);
                     auto msg = Message::Read(decoder, m_get_entry_type);
                     if (!msg && decoder.error())
                       DEBUG("error reading in handshake: " << decoder.error());
                     return msg;
                   },
                   [&](llvm::ArrayRef<std::shared_ptr<Message>> msgs) {
                     m_outgoing.emplace(msgs);
                   })) {
    m_state = static_cast<int>(kDead);
    m_active = false;
    return;
  }

  m_state = static_cast<int>(kActive);
  while (m_active) {
    if (!m_stream)
      break;
    decoder.set_proto_rev(m_proto_rev);
    decoder.Reset();
    auto msg = Message::Read(decoder, m_get_entry_type);
    if (!msg) {
      // terminate connection on bad message
      if (m_stream) m_stream->close();
      break;
    }
    m_process_incoming(std::move(msg), this, m_proto_rev);
  }
  m_state = static_cast<int>(kDead);
  m_active = false;
}

void NetworkConnection::WriteThreadMain() {
  WireEncoder encoder(m_proto_rev);

  while (m_active) {
    auto msgs = m_outgoing.pop();
    DEBUG4("write thread woke up");
    if (msgs.empty()) break;
    encoder.set_proto_rev(m_proto_rev);
    encoder.Reset();
    DEBUG4("sending " << msgs.size() << " messages");
    for (auto& msg : msgs) {
      if (msg) msg->Write(encoder);
    }
    TCPStream::Error err;
    if (!m_stream) break;
    if (encoder.size() == 0) continue;
    if (m_stream->send(encoder.data(), encoder.size(), &err) == 0) break;
    DEBUG4("sent " << encoder.size() << " bytes");
  }
  m_state = static_cast<int>(kDead);
  m_active = false;
}
