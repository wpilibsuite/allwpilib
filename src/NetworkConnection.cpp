/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NetworkConnection.h"

#include "support/timestamp.h"
#include "tcpsockets/NetworkStream.h"
#include "Log.h"
#include "Notifier.h"
#include "raw_socket_istream.h"
#include "WireDecoder.h"
#include "WireEncoder.h"

using namespace nt;

std::atomic_uint NetworkConnection::s_uid;

NetworkConnection::NetworkConnection(std::unique_ptr<NetworkStream> stream,
                                     Notifier& notifier,
                                     HandshakeFunc handshake,
                                     Message::GetEntryTypeFunc get_entry_type)
    : m_uid(s_uid.fetch_add(1)),
      m_stream(std::move(stream)),
      m_notifier(notifier),
      m_handshake(handshake),
      m_get_entry_type(get_entry_type) {
  m_active = false;
  m_proto_rev = 0x0300;
  m_state = static_cast<int>(kCreated);
  m_last_update = 0;
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

ConnectionInfo NetworkConnection::info() const {
  return ConnectionInfo{remote_id(), m_stream->getPeerIP(),
                        static_cast<unsigned int>(m_stream->getPeerPort()),
                        m_last_update, m_proto_rev};
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
  m_notifier.NotifyConnection(true, info());
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
    DEBUG4("received type=" << msg->type() << " with str=" << msg->str()
                            << " id=" << msg->id()
                            << " seq_num=" << msg->seq_num_uid());
    m_last_update = Now();
    m_process_incoming(std::move(msg), this);
  }
  DEBUG3("read thread died");
  if (m_state != kDead) m_notifier.NotifyConnection(false, info());
  m_state = static_cast<int>(kDead);
  m_active = false;
  m_outgoing.push(Outgoing());  // also kill write thread
}

void NetworkConnection::WriteThreadMain() {
  WireEncoder encoder(m_proto_rev);

  while (m_active) {
    auto msgs = m_outgoing.pop();
    DEBUG4("write thread woke up");
    if (msgs.empty()) continue;
    encoder.set_proto_rev(m_proto_rev);
    encoder.Reset();
    DEBUG3("sending " << msgs.size() << " messages");
    for (auto& msg : msgs) {
      if (msg) {
        DEBUG4("sending type=" << msg->type() << " with str=" << msg->str()
                               << " id=" << msg->id()
                               << " seq_num=" << msg->seq_num_uid());
        msg->Write(encoder);
      }
    }
    NetworkStream::Error err;
    if (!m_stream) break;
    if (encoder.size() == 0) continue;
    if (m_stream->send(encoder.data(), encoder.size(), &err) == 0) break;
    DEBUG4("sent " << encoder.size() << " bytes");
  }
  DEBUG3("write thread died");
  if (m_state != kDead) m_notifier.NotifyConnection(false, info());
  m_state = static_cast<int>(kDead);
  m_active = false;
  if (m_stream) m_stream->close();  // also kill read thread
}

void NetworkConnection::QueueOutgoing(std::shared_ptr<Message> msg) {
  std::lock_guard<std::mutex> lock(m_pending_mutex);

  // Merge with previous.  One case we don't combine: delete/assign loop.
  switch (msg->type()) {
    case Message::kEntryAssign:
    case Message::kEntryUpdate: {
      // don't do this for unassigned id's
      unsigned int id = msg->id();
      if (id == 0xffff) {
        m_pending_outgoing.push_back(msg);
        break;
      }
      if (id < m_pending_update.size() && m_pending_update[id].first != 0) {
        // overwrite the previous one for this id
        auto& oldmsg = m_pending_outgoing[m_pending_update[id].first - 1];
        if (oldmsg && oldmsg->Is(Message::kEntryAssign) &&
            msg->Is(Message::kEntryUpdate)) {
          // need to update assignment with new seq_num and value
          oldmsg = Message::EntryAssign(oldmsg->str(), id, msg->seq_num_uid(),
                                        msg->value(), oldmsg->flags());
        } else
          oldmsg = msg;  // easy update
      } else {
        // new, but remember it
        std::size_t pos = m_pending_outgoing.size();
        m_pending_outgoing.push_back(msg);
        if (id >= m_pending_update.size()) m_pending_update.resize(id + 1);
        m_pending_update[id].first = pos + 1;
      }
      break;
    }
    case Message::kEntryDelete: {
      // don't do this for unassigned id's
      unsigned int id = msg->id();
      if (id == 0xffff) {
        m_pending_outgoing.push_back(msg);
        break;
      }

      // clear previous updates
      if (id < m_pending_update.size()) {
        if (m_pending_update[id].first != 0) {
          m_pending_outgoing[m_pending_update[id].first - 1].reset();
          m_pending_update[id].first = 0;
        }
        if (m_pending_update[id].second != 0) {
          m_pending_outgoing[m_pending_update[id].second - 1].reset();
          m_pending_update[id].second = 0;
        }
      }

      // add deletion
      m_pending_outgoing.push_back(msg);
      break;
    }
    case Message::kFlagsUpdate: {
      // don't do this for unassigned id's
      unsigned int id = msg->id();
      if (id == 0xffff) {
        m_pending_outgoing.push_back(msg);
        break;
      }
      if (id < m_pending_update.size() && m_pending_update[id].second != 0) {
        // overwrite the previous one for this id
        m_pending_outgoing[m_pending_update[id].second - 1] = msg;
      } else {
        // new, but remember it
        std::size_t pos = m_pending_outgoing.size();
        m_pending_outgoing.push_back(msg);
        if (id >= m_pending_update.size()) m_pending_update.resize(id + 1);
        m_pending_update[id].second = pos + 1;
      }
      break;
    }
    case Message::kClearEntries: {
      // knock out all previous assigns/updates!
      for (auto& i : m_pending_outgoing) {
        if (!i) continue;
        auto t = i->type();
        if (t == Message::kEntryAssign || t == Message::kEntryUpdate ||
            t == Message::kFlagsUpdate || t == Message::kEntryDelete ||
            t == Message::kClearEntries)
          i.reset();
      }
      m_pending_update.resize(0);
      m_pending_outgoing.push_back(msg);
      break;
    }
    default:
      m_pending_outgoing.push_back(msg);
      break;
  }
}

void NetworkConnection::PostOutgoing() {
  std::lock_guard<std::mutex> lock(m_pending_mutex);
  if (m_pending_outgoing.empty()) return;
  m_outgoing.emplace(std::move(m_pending_outgoing));
  m_pending_outgoing.resize(0);
  m_pending_update.resize(0);
}
