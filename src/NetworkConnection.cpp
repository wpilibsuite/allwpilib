/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NetworkConnection.h"

#include "tcpsockets/TCPStream.h"
#include "raw_socket_istream.h"
#include "WireDecoder.h"
#include "WireEncoder.h"

using namespace nt;

NetworkConnection::NetworkConnection(std::unique_ptr<TCPStream> stream,
                                     Message::GetEntryTypeFunc get_entry_type)
    : m_stream(std::move(stream)),
      m_get_entry_type(get_entry_type),
      m_active(false),
      m_proto_rev(0x0300) {}

NetworkConnection::~NetworkConnection() { Stop(); }

void NetworkConnection::Start() {
  if (m_active) return;
  m_active = true;
  m_write_thread = std::thread(&NetworkConnection::WriteThreadMain, this);
  m_read_thread = std::thread(&NetworkConnection::ReadThreadMain, this);
}

void NetworkConnection::Stop() {
  if (m_stream) m_stream->close();
  m_active = false;
  if (m_write_thread.joinable()) m_write_thread.join();
  if (m_read_thread.joinable()) m_read_thread.join();
}

void NetworkConnection::ReadThreadMain() {
  raw_socket_istream is(*m_stream);
  WireDecoder decoder(is, m_proto_rev);

  while (m_active) {
    if (!m_stream)
      break;
    decoder.set_proto_rev(m_proto_rev);
    decoder.Reset();
    auto msg = Message::Read(decoder, m_get_entry_type);
    if (!msg) {
      // terminate connection on bad message
      m_stream->close();
      break;
    }
    m_incoming.push(msg);
  }
  m_active = false;
}

void NetworkConnection::WriteThreadMain() {
  WireEncoder encoder(m_proto_rev);

  while (m_active) {
    auto msgs = m_outgoing.pop();
    encoder.set_proto_rev(m_proto_rev);
    encoder.Reset();
    for (auto& msg : msgs) msg->Write(encoder);
    TCPStream::Error err;
    if (!m_stream) break;
    if (m_stream->send(encoder.data(), encoder.size(), &err) == 0) break;
  }
  m_active = false;
}
