/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Dispatcher.h"

#include "tcpsockets/TCPAcceptor.h"
#include "tcpsockets/TCPConnector.h"

using namespace nt;

#define DEBUG(str) puts(str)

std::unique_ptr<Dispatcher> Dispatcher::m_instance;

Dispatcher::Dispatcher()
    : m_server(false),
      m_active(false),
      m_update_rate(100),
      m_do_flush(false),
      m_do_reconnect(false) {}

Dispatcher::~Dispatcher() { Stop(); }

void Dispatcher::StartServer(const char* listen_address, unsigned int port) {
  {
    std::lock_guard<std::mutex> lock(m_user_mutex);
    if (m_active) return;
    m_active = true;
  }
  m_server = true;
  m_dispatch_thread = std::thread(&Dispatcher::DispatchThreadMain, this);
  m_clientserver_thread =
      std::thread(&Dispatcher::ServerThreadMain, this, listen_address, port);
}

void Dispatcher::StartClient(const char* server_name, unsigned int port) {
  {
    std::lock_guard<std::mutex> lock(m_user_mutex);
    if (m_active) return;
    m_active = true;
  }
  m_server = false;
  m_dispatch_thread = std::thread(&Dispatcher::DispatchThreadMain, this);
  m_clientserver_thread =
      std::thread(&Dispatcher::ClientThreadMain, this, server_name, port);
}

void Dispatcher::Stop() {
  {
    std::lock_guard<std::mutex> lock(m_user_mutex);
    m_active = false;

    // close all connections
    for (auto& conn : m_connections) conn.net->Stop();
  }

  // wake up dispatch thread with a flush
  m_flush_cv.notify_one();

  // wake up client thread with a reconnect
  ClientReconnect();

  // wake up server thread by shutting down the socket
  if (m_server_acceptor) m_server_acceptor->shutdown();

  // join threads
  if (m_dispatch_thread.joinable()) m_dispatch_thread.join();
  if (m_clientserver_thread.joinable()) m_clientserver_thread.join();
}

void Dispatcher::SetUpdateRate(double interval) {
  // don't allow update rates faster than 100 ms
  if (interval < 0.1)
    interval = 0.1;
  m_update_rate = static_cast<unsigned int>(interval * 1000);
}

void Dispatcher::SetIdentity(llvm::StringRef name) {
  std::lock_guard<std::mutex> lock(m_user_mutex);
  m_identity = name;
}

void Dispatcher::Flush() {
  auto now = std::chrono::steady_clock::now();
  {
    std::lock_guard<std::mutex> lock(m_flush_mutex);
    // don't allow flushes more often than every 100 ms
    if ((now - m_last_flush) < std::chrono::milliseconds(100))
      return;
    m_last_flush = now;
    m_do_flush = true;
  }
  m_flush_cv.notify_one();
}

void Dispatcher::DispatchThreadMain() {
  auto timeout_time = std::chrono::steady_clock::now();
  int count = 0;
  while (m_active) {
    // handle loop taking too long
    auto start = std::chrono::steady_clock::now();
    if (start > timeout_time)
      timeout_time = start;

    // wait for periodic or when flushed
    timeout_time += std::chrono::milliseconds(m_update_rate);
    std::unique_lock<std::mutex> lock(m_flush_mutex);
    m_reconnect_cv.wait_until(lock, timeout_time,
                              [&] { return !m_active || m_do_flush; });
    m_do_flush = false;
    lock.unlock();
    if (!m_active) break;  // in case we were woken up to terminate

    if (++count > 10) {
      DEBUG("dispatch running");
      count = 0;
    }
  }
}

void Dispatcher::ServerThreadMain(const char* listen_address,
                                  unsigned int port) {
  m_server_acceptor.reset(
      new TCPAcceptor(static_cast<int>(port), listen_address));
  if (m_server_acceptor->start() != 0) {
    m_active = false;
    return;
  }
  while (m_active) {
    auto stream = m_server_acceptor->accept();
    if (!stream) {
      m_active = false;
      break;
    }
    DEBUG("server got a connection");

    // add to connections list
    Connection conn;
    conn.net.reset(new NetworkConnection(
        std::move(stream),
        [this](unsigned int id) { return GetEntryType(id); }));
    conn.net->Start();
    AddConnection(std::move(conn));
  }
}

void Dispatcher::ClientThreadMain(const char* server_name, unsigned int port) {
  unsigned int proto_rev = 0x0300;
  while (m_active) {
    // get identity
    std::string self_id;
    {
      std::lock_guard<std::mutex> lock(m_user_mutex);
      self_id = m_identity;
    }

    // sleep between retries
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // try to connect (with timeout)
    DEBUG("client trying to connect");
    auto stream = TCPConnector::connect(server_name, static_cast<int>(port), 1);
    if (!stream) continue;  // keep retrying
    DEBUG("client connected");

    Connection conn;
    conn.net.reset(new NetworkConnection(
        std::move(stream),
        [this](unsigned int id) { return GetEntryType(id); }));
    conn.net->set_proto_rev(proto_rev);
    conn.net->Start();

    // send client hello
    DEBUG("client sending hello");
    conn.net->outgoing().push(
        NetworkConnection::Outgoing{Message::ClientHello(self_id)});

    // wait for response
    auto msg = conn.net->incoming().pop();
    if (!msg) {
      // disconnected, retry
      DEBUG("client disconnected waiting for first response");
      proto_rev = 0x0300;
      continue;
    }

    if (msg->Is(Message::kProtoUnsup)) {
      // reconnect with lower protocol (if possible)
      if (proto_rev <= 0x0200) {
        // no more options, abort (but keep trying to connect)
        proto_rev = 0x0300;
        continue;
      }
      proto_rev = 0x0200;
      continue;
    }

    if (proto_rev >= 0x0300) {
      // should be server hello; if not, disconnect, but keep trying to connect
      if (!msg->Is(Message::kServerHello)) continue;
      conn.remote_id = msg->str();
      // get the next message (blocks)
      msg = conn.net->incoming().pop();
    }

    // receive initial assignments
    std::vector<std::shared_ptr<Message>> incoming;
    for (;;) {
      if (!msg) {
        // disconnected, retry
        DEBUG("client disconnected waiting for initial entries");
        proto_rev = 0x0300;
        continue;
      }
      if (msg->Is(Message::kServerHelloDone)) break;
      if (!msg->Is(Message::kEntryAssign)) {
        // unexpected message
        DEBUG("received message other than entry assignment during initial handshake");
        proto_rev = 0x0300;
        continue;
      }
      incoming.push_back(msg);
      // get the next message (blocks)
      msg = conn.net->incoming().pop();
    }

    // generate outgoing assignments
    NetworkConnection::Outgoing outgoing;

    if (proto_rev >= 0x0300)
      outgoing.push_back(Message::ClientHelloDone());

    if (!outgoing.empty())
      conn.net->outgoing().push(std::move(outgoing));

    // add to connections list (the dispatcher thread will handle from here)
    AddConnection(std::move(conn));

    // block until told to reconnect
    std::unique_lock<std::mutex> lock(m_reconnect_mutex);
    m_reconnect_cv.wait(lock, [&] { return m_do_reconnect; });
    m_do_reconnect = false;
    lock.unlock();
  }
}

void Dispatcher::ClientReconnect() {
  if (m_server) return;
  {
    std::lock_guard<std::mutex> lock(m_reconnect_mutex);
    m_do_reconnect = true;
  }
  m_reconnect_cv.notify_one();
}

void Dispatcher::AddConnection(Connection&& conn) {
  std::lock_guard<std::mutex> lock(m_user_mutex);
  m_connections.push_back(std::move(conn));
}

NT_Type Dispatcher::GetEntryType(unsigned int id) const {
  std::lock_guard<std::mutex> lock(m_idmap_mutex);
  if (id >= m_idmap.size()) return NT_UNASSIGNED;
  auto value = m_idmap[id]->value();
  if (!value) return NT_UNASSIGNED;
  return value->type();
}
