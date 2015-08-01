/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Dispatcher.h"

#include <algorithm>
#include <iterator>

#include "tcpsockets/TCPAcceptor.h"
#include "tcpsockets/TCPConnector.h"
#include "Log.h"

using namespace nt;

ATOMIC_STATIC_INIT(Dispatcher)

Dispatcher::Dispatcher()
    : m_server(false),
      m_do_flush(false),
      m_reconnect_proto_rev(0x0300),
      m_do_reconnect(true) {
  m_active = false;
  m_update_rate = 100;
}

Dispatcher::~Dispatcher() {
  Stop();
}

void Dispatcher::StartServer(const char* listen_address, unsigned int port) {
  {
    std::lock_guard<std::mutex> lock(m_user_mutex);
    if (m_active) return;
    m_active = true;
  }
  m_server = true;

  using namespace std::placeholders;
  Storage::GetInstance().SetOutgoing(
      std::bind(&Dispatcher::QueueOutgoing, this, _1, _2, _3), m_server);

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

  using namespace std::placeholders;
  Storage::GetInstance().SetOutgoing(
      std::bind(&Dispatcher::QueueOutgoing, this, _1, _2, _3), m_server);

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
  // local copy of active m_connections
  struct ConnectionRef {
    NetworkConnection* net;
    NetworkConnection::Outgoing outgoing;
  };
  std::vector<ConnectionRef> connections;

  auto timeout_time = std::chrono::steady_clock::now();
  int count = 0;
  std::unique_lock<std::mutex> flush_lock(m_flush_mutex);
  while (m_active) {
    // handle loop taking too long
    auto start = std::chrono::steady_clock::now();
    if (start > timeout_time)
      timeout_time = start;

    // wait for periodic or when flushed
    timeout_time += std::chrono::milliseconds(m_update_rate);
    m_reconnect_cv.wait_until(flush_lock, timeout_time,
                              [&] { return !m_active || m_do_flush; });
    m_do_flush = false;
    if (!m_active) break;  // in case we were woken up to terminate

    if (++count > 10) {
      DEBUG("dispatch running");
      count = 0;
    }

    // make a local copy of the connections list (so we don't hold the lock)
    connections.resize(0);
    {
      std::lock_guard<std::mutex> user_lock(m_user_mutex);
      bool reconnect = false;
      for (auto& conn : m_connections) {
        if (conn.net->state() == NetworkConnection::kActive) {
          connections.push_back(ConnectionRef());
          connections.back().net = conn.net.get();
          connections.back().outgoing.swap(conn.outgoing);
        }
        if (!m_server && conn.net->state() == NetworkConnection::kDead)
          reconnect = true;
      }
      // reconnect if we disconnected (and a reconnect is not in progress)
      if (reconnect && !m_do_reconnect) {
        m_do_reconnect = true;
        m_reconnect_cv.notify_one();
      }
    }

    // scan outgoing messages to remove unnecessary updates

    // send outgoing messages
    for (auto& conn : connections) {
      if (!conn.outgoing.empty())
        conn.net->outgoing().emplace(std::move(conn.outgoing));
    }
  }
}

void Dispatcher::QueueOutgoing(std::shared_ptr<Message> msg,
                               NetworkConnection* only,
                               NetworkConnection* except) {
  std::lock_guard<std::mutex> user_lock(m_user_mutex);
  for (auto& conn : m_connections) {
    if (conn.net.get() == except) continue;
    if (only && conn.net.get() != only) continue;
    auto state = conn.net->state();
    if (state != NetworkConnection::kSynchronized &&
        state != NetworkConnection::kActive) continue;
    conn.outgoing.push_back(msg);
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
    DEBUG("server: client connection from " << stream->getPeerIP() << " port "
                                            << stream->getPeerPort());

    // add to connections list
    using namespace std::placeholders;
    Storage& storage = Storage::GetInstance();
    std::unique_ptr<NetworkConnection> conn_unique(new NetworkConnection(
        std::move(stream),
        std::bind(&Dispatcher::ServerHandshake, this, _1, _2, _3),
        std::bind(&Storage::GetEntryType, &storage, _1),
        std::bind(&Storage::ProcessIncoming, &storage, _1, _2)));
    auto conn = conn_unique.get();
    {
      std::lock_guard<std::mutex> lock(m_user_mutex);
      m_connections.emplace_back(std::move(conn_unique));
    }
    conn->Start();
  }
}

void Dispatcher::ClientThreadMain(const char* server_name, unsigned int port) {
  while (m_active) {
    // sleep between retries
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // try to connect (with timeout)
    DEBUG("client trying to connect");
    auto stream = TCPConnector::connect(server_name, static_cast<int>(port), 1);
    if (!stream) continue;  // keep retrying
    DEBUG("client connected");

    std::unique_lock<std::mutex> lock(m_user_mutex);
    using namespace std::placeholders;
    Storage& storage = Storage::GetInstance();
    std::unique_ptr<NetworkConnection> conn_unique(new NetworkConnection(
        std::move(stream),
        std::bind(&Dispatcher::ClientHandshake, this, _1, _2, _3),
        std::bind(&Storage::GetEntryType, &storage, _1),
        std::bind(&Storage::ProcessIncoming, &storage, _1, _2)));
    auto conn = conn_unique.get();
    m_connections.resize(0);  // disconnect any current
    m_connections.emplace_back(std::move(conn_unique));
    conn->set_proto_rev(m_reconnect_proto_rev);
    conn->Start();

    // block until told to reconnect
    m_do_reconnect = false;
    m_reconnect_cv.wait(lock, [&] { return m_do_reconnect; });
  }
}

bool Dispatcher::ClientHandshake(
    NetworkConnection& conn,
    std::function<std::shared_ptr<Message>()> get_msg,
    std::function<void(llvm::ArrayRef<std::shared_ptr<Message>>)> send_msgs) {
  // get identity
  std::string self_id;
  {
    std::lock_guard<std::mutex> lock(m_user_mutex);
    self_id = m_identity;
  }

  // send client hello
  DEBUG("client: sending hello");
  send_msgs(Message::ClientHello(self_id));

  // wait for response
  auto msg = get_msg();
  if (!msg) {
    // disconnected, retry
    DEBUG("client: server disconnected before first response");
    return false;
  }

  if (msg->Is(Message::kProtoUnsup)) {
    if (msg->id() == 0x0200) ClientReconnect(0x0200);
    return false;
  }

  bool new_server = true;
  if (conn.proto_rev() >= 0x0300) {
    // should be server hello; if not, disconnect.
    if (!msg->Is(Message::kServerHello)) return false;
    conn.set_remote_id(msg->str());
    if ((msg->flags() & 1) != 0) new_server = false;
    // get the next message
    msg = get_msg();
  }

  // receive initial assignments
  std::vector<std::shared_ptr<Message>> incoming;
  for (;;) {
    if (!msg) {
      // disconnected, retry
      DEBUG("client: server disconnected during initial entries");
      return false;
    }
    if (msg->Is(Message::kServerHelloDone)) break;
    if (!msg->Is(Message::kEntryAssign)) {
      // unexpected message
      DEBUG("client: received message (" << msg->type() << ") other than entry assignment during initial handshake");
      return false;
    }
    incoming.emplace_back(std::move(msg));
    // get the next message
    msg = get_msg();
  }

  // generate outgoing assignments
  NetworkConnection::Outgoing outgoing;

  Storage::GetInstance().ApplyInitialAssignments(conn, incoming, new_server,
                                                 &outgoing);

  if (conn.proto_rev() >= 0x0300)
    outgoing.emplace_back(Message::ClientHelloDone());

  if (!outgoing.empty()) send_msgs(outgoing);

  INFO("client: CONNECTED to server " << conn.stream().getPeerIP() << " port "
                                      << conn.stream().getPeerPort());
  return true;
}

bool Dispatcher::ServerHandshake(
    NetworkConnection& conn,
    std::function<std::shared_ptr<Message>()> get_msg,
    std::function<void(llvm::ArrayRef<std::shared_ptr<Message>>)> send_msgs) {
  // Wait for the client to send us a hello.
  auto msg = get_msg();
  if (!msg) {
    DEBUG("server: client disconnected before sending hello");
    return false;
  }
  if (!msg->Is(Message::kClientHello)) {
    DEBUG("server: client initial message was not client hello");
    return false;
  }

  // Check that the client requested version is not too high.
  unsigned int proto_rev = msg->id();
  if (proto_rev > 0x0300) {
    DEBUG("server: client requested proto > 0x0300");
    send_msgs(Message::ProtoUnsup());
    return false;
  }

  if (proto_rev >= 0x0300) conn.set_remote_id(msg->str());

  // Set the proto version to the client requested version
  conn.set_proto_rev(proto_rev);

  // Send initial set of assignments
  NetworkConnection::Outgoing outgoing;

  // Start with server hello.  TODO: initial connection flag
  if (proto_rev >= 0x0300) {
    std::lock_guard<std::mutex> lock(m_user_mutex);
    outgoing.emplace_back(Message::ServerHello(0u, m_identity));
  }

  // Get snapshot of initial assignments
  Storage::GetInstance().GetInitialAssignments(conn, &outgoing);

  // Finish with server hello done
  outgoing.emplace_back(Message::ServerHelloDone());

  // Batch transmit
  DEBUG("server: sending initial assignments");
  send_msgs(outgoing);

  // In proto rev 3.0 and later, the handshake concludes with a client hello
  // done message, so we can batch the assigns before marking the connection
  // active.  In pre-3.0, we need to just immediately mark it active and hand
  // off control to the dispatcher to assign them as they arrive.
  if (proto_rev >= 0x0300) {
    // receive client initial assignments
    std::vector<std::shared_ptr<Message>> incoming;
    msg = get_msg();
    for (;;) {
      if (!msg) {
        // disconnected, retry
        DEBUG("server: disconnected waiting for initial entries");
        return false;
      }
      if (msg->Is(Message::kClientHelloDone)) break;
      if (!msg->Is(Message::kEntryAssign)) {
        // unexpected message
        DEBUG("server: received message (" << msg->type() << ") other than entry assignment during initial handshake");
        return false;
      }
      incoming.push_back(msg);
      // get the next message (blocks)
      msg = get_msg();
    }
    Storage& storage = Storage::GetInstance();
    for (auto& msg : incoming) storage.ProcessIncoming(msg, &conn);
  }

  INFO("server: client CONNECTED: " << conn.stream().getPeerIP() << " port "
                                    << conn.stream().getPeerPort());
  return true;
}

void Dispatcher::ClientReconnect(unsigned int proto_rev) {
  if (m_server) return;
  {
    std::lock_guard<std::mutex> lock(m_user_mutex);
    m_reconnect_proto_rev = proto_rev;
    m_do_reconnect = true;
  }
  m_reconnect_cv.notify_one();
}
