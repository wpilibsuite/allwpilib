/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Dispatcher.h"

#include <algorithm>
#include <iterator>

#include "Log.h"
#include "tcpsockets/TCPAcceptor.h"
#include "tcpsockets/TCPConnector.h"

using namespace nt;

ATOMIC_STATIC_INIT(Dispatcher)

void Dispatcher::StartServer(llvm::StringRef persist_filename,
                             const char* listen_address, unsigned int port) {
  DispatcherBase::StartServer(
      persist_filename,
      std::unique_ptr<wpi::NetworkAcceptor>(new wpi::TCPAcceptor(
          static_cast<int>(port), listen_address, Logger::GetInstance())));
}

void Dispatcher::SetServer(const char* server_name, unsigned int port) {
  std::string server_name_copy(server_name);
  SetConnector([=]() -> std::unique_ptr<wpi::NetworkStream> {
    return wpi::TCPConnector::connect(server_name_copy.c_str(),
                                      static_cast<int>(port),
                                      Logger::GetInstance(), 1);
  });
}

void Dispatcher::SetServer(
    ArrayRef<std::pair<StringRef, unsigned int>> servers) {
  std::vector<Connector> connectors;
  for (const auto& server : servers) {
    std::string server_name(server.first);
    unsigned int port = server.second;
    connectors.emplace_back([=]() -> std::unique_ptr<wpi::NetworkStream> {
      return wpi::TCPConnector::connect(server_name.c_str(),
                                        static_cast<int>(port),
                                        Logger::GetInstance(), 1);
    });
  }
  SetConnector(std::move(connectors));
}

void Dispatcher::SetServerOverride(const char* server_name, unsigned int port) {
  std::string server_name_copy(server_name);
  SetConnectorOverride([=]() -> std::unique_ptr<wpi::NetworkStream> {
    return wpi::TCPConnector::connect(server_name_copy.c_str(),
                                      static_cast<int>(port),
                                      Logger::GetInstance(), 1);
  });
}

void Dispatcher::ClearServerOverride() { ClearConnectorOverride(); }

Dispatcher::Dispatcher()
    : Dispatcher(Storage::GetInstance(), Notifier::GetInstance()) {}

DispatcherBase::DispatcherBase(Storage& storage, Notifier& notifier)
    : m_storage(storage), m_notifier(notifier) {
  m_active = false;
  m_update_rate = 100;
}

DispatcherBase::~DispatcherBase() {
  Logger::GetInstance().SetLogger(nullptr);
  Stop();
}

void DispatcherBase::StartServer(
    StringRef persist_filename,
    std::unique_ptr<wpi::NetworkAcceptor> acceptor) {
  {
    std::lock_guard<std::mutex> lock(m_user_mutex);
    if (m_active) return;
    m_active = true;
  }
  m_server = true;
  m_persist_filename = persist_filename;
  m_server_acceptor = std::move(acceptor);

  // Load persistent file.  Ignore errors, but pass along warnings.
  if (!persist_filename.empty()) {
    bool first = true;
    m_storage.LoadPersistent(
        persist_filename, [&](std::size_t line, const char* msg) {
          if (first) {
            first = false;
            WARNING("When reading initial persistent values from '"
                    << persist_filename << "':");
          }
          WARNING(persist_filename << ":" << line << ": " << msg);
        });
  }

  using namespace std::placeholders;
  m_storage.SetOutgoing(std::bind(&Dispatcher::QueueOutgoing, this, _1, _2, _3),
                        m_server);

  m_dispatch_thread = std::thread(&Dispatcher::DispatchThreadMain, this);
  m_clientserver_thread = std::thread(&Dispatcher::ServerThreadMain, this);
}

void DispatcherBase::StartClient() {
  {
    std::lock_guard<std::mutex> lock(m_user_mutex);
    if (m_active) return;
    m_active = true;
  }
  m_server = false;
  using namespace std::placeholders;
  m_storage.SetOutgoing(std::bind(&Dispatcher::QueueOutgoing, this, _1, _2, _3),
                        m_server);

  m_dispatch_thread = std::thread(&Dispatcher::DispatchThreadMain, this);
  m_clientserver_thread = std::thread(&Dispatcher::ClientThreadMain, this);
}

void DispatcherBase::Stop() {
  m_active = false;

  // wake up dispatch thread with a flush
  m_flush_cv.notify_one();

  // wake up client thread with a reconnect
  {
    std::lock_guard<std::mutex> lock(m_user_mutex);
    m_client_connectors.resize(0);
  }
  ClientReconnect();

  // wake up server thread by shutting down the socket
  if (m_server_acceptor) m_server_acceptor->shutdown();

  // join threads, with timeout
  if (m_dispatch_thread.joinable()) m_dispatch_thread.join();
  if (m_clientserver_thread.joinable()) m_clientserver_thread.join();

  std::vector<std::shared_ptr<NetworkConnection>> conns;
  {
    std::lock_guard<std::mutex> lock(m_user_mutex);
    conns.swap(m_connections);
  }

  // close all connections
  conns.resize(0);
}

void DispatcherBase::SetUpdateRate(double interval) {
  // don't allow update rates faster than 10 ms or slower than 1 second
  if (interval < 0.01)
    interval = 0.01;
  else if (interval > 1.0)
    interval = 1.0;
  m_update_rate = static_cast<unsigned int>(interval * 1000);
}

void DispatcherBase::SetIdentity(llvm::StringRef name) {
  std::lock_guard<std::mutex> lock(m_user_mutex);
  m_identity = name;
}

void DispatcherBase::Flush() {
  auto now = std::chrono::steady_clock::now();
  {
    std::lock_guard<std::mutex> lock(m_flush_mutex);
    // don't allow flushes more often than every 10 ms
    if ((now - m_last_flush) < std::chrono::milliseconds(10)) return;
    m_last_flush = now;
    m_do_flush = true;
  }
  m_flush_cv.notify_one();
}

std::vector<ConnectionInfo> DispatcherBase::GetConnections() const {
  std::vector<ConnectionInfo> conns;
  if (!m_active) return conns;

  std::lock_guard<std::mutex> lock(m_user_mutex);
  for (auto& conn : m_connections) {
    if (conn->state() != NetworkConnection::kActive) continue;
    conns.emplace_back(conn->info());
  }

  return conns;
}

void DispatcherBase::NotifyConnections(
    ConnectionListenerCallback callback) const {
  std::lock_guard<std::mutex> lock(m_user_mutex);
  for (const auto& conn : m_connections) conn->NotifyIfActive(callback);
}

void DispatcherBase::SetConnector(Connector connector) {
  std::vector<Connector> connectors;
  connectors.push_back(connector);
  SetConnector(std::move(connectors));
}

void DispatcherBase::SetConnector(std::vector<Connector>&& connectors) {
  std::lock_guard<std::mutex> lock(m_user_mutex);
  m_client_connectors = std::move(connectors);
}

void DispatcherBase::SetConnectorOverride(Connector connector) {
  std::lock_guard<std::mutex> lock(m_user_mutex);
  m_client_connector_override = std::move(connector);
}

void DispatcherBase::ClearConnectorOverride() {
  std::lock_guard<std::mutex> lock(m_user_mutex);
  m_client_connector_override = nullptr;
}

void DispatcherBase::DispatchThreadMain() {
  auto timeout_time = std::chrono::steady_clock::now();

  static const auto save_delta_time = std::chrono::seconds(1);
  auto next_save_time = timeout_time + save_delta_time;

  int count = 0;

  while (m_active) {
    // handle loop taking too long
    auto start = std::chrono::steady_clock::now();
    if (start > timeout_time) timeout_time = start;

    // wait for periodic or when flushed
    timeout_time += std::chrono::milliseconds(m_update_rate);
    std::unique_lock<std::mutex> flush_lock(m_flush_mutex);
    m_flush_cv.wait_until(flush_lock, timeout_time,
                          [&] { return !m_active || m_do_flush; });
    m_do_flush = false;
    flush_lock.unlock();
    if (!m_active) break;  // in case we were woken up to terminate

    // perform periodic persistent save
    if (m_server && !m_persist_filename.empty() && start > next_save_time) {
      next_save_time += save_delta_time;
      // handle loop taking too long
      if (start > next_save_time) next_save_time = start + save_delta_time;
      const char* err = m_storage.SavePersistent(m_persist_filename, true);
      if (err) WARNING("periodic persistent save: " << err);
    }

    {
      std::lock_guard<std::mutex> user_lock(m_user_mutex);
      bool reconnect = false;

      if (++count > 10) {
        DEBUG("dispatch running " << m_connections.size() << " connections");
        count = 0;
      }

      for (auto& conn : m_connections) {
        // post outgoing messages if connection is active
        // only send keep-alives on client
        if (conn->state() == NetworkConnection::kActive)
          conn->PostOutgoing(!m_server);

        // if client, reconnect if connection died
        if (!m_server && conn->state() == NetworkConnection::kDead)
          reconnect = true;
      }
      // reconnect if we disconnected (and a reconnect is not in progress)
      if (reconnect && !m_do_reconnect) {
        m_do_reconnect = true;
        m_reconnect_cv.notify_one();
      }
    }
  }
}

void DispatcherBase::QueueOutgoing(std::shared_ptr<Message> msg,
                                   NetworkConnection* only,
                                   NetworkConnection* except) {
  std::lock_guard<std::mutex> user_lock(m_user_mutex);
  for (auto& conn : m_connections) {
    if (conn.get() == except) continue;
    if (only && conn.get() != only) continue;
    auto state = conn->state();
    if (state != NetworkConnection::kSynchronized &&
        state != NetworkConnection::kActive)
      continue;
    conn->QueueOutgoing(msg);
  }
}

void DispatcherBase::ServerThreadMain() {
  if (m_server_acceptor->start() != 0) {
    m_active = false;
    return;
  }
  while (m_active) {
    auto stream = m_server_acceptor->accept();
    if (!stream) {
      m_active = false;
      return;
    }
    if (!m_active) return;
    DEBUG("server: client connection from " << stream->getPeerIP() << " port "
                                            << stream->getPeerPort());

    // add to connections list
    using namespace std::placeholders;
    auto conn = std::make_shared<NetworkConnection>(
        std::move(stream), m_notifier,
        std::bind(&Dispatcher::ServerHandshake, this, _1, _2, _3),
        std::bind(&Storage::GetEntryType, &m_storage, _1));
    conn->set_process_incoming(
        std::bind(&Storage::ProcessIncoming, &m_storage, _1, _2,
                  std::weak_ptr<NetworkConnection>(conn)));
    {
      std::lock_guard<std::mutex> lock(m_user_mutex);
      // reuse dead connection slots
      bool placed = false;
      for (auto& c : m_connections) {
        if (c->state() == NetworkConnection::kDead) {
          c = conn;
          placed = true;
          break;
        }
      }
      if (!placed) m_connections.emplace_back(conn);
      conn->Start();
    }
  }
}

void DispatcherBase::ClientThreadMain() {
  std::size_t i = 0;
  while (m_active) {
    // sleep between retries
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    Connector connect;

    // get next server to connect to
    {
      std::lock_guard<std::mutex> lock(m_user_mutex);
      if (m_client_connector_override) {
        connect = m_client_connector_override;
      } else {
        if (m_client_connectors.empty()) continue;
        if (i >= m_client_connectors.size()) i = 0;
        connect = m_client_connectors[i++];
      }
    }

    // try to connect (with timeout)
    DEBUG("client trying to connect");
    auto stream = connect();
    if (!stream) continue;  // keep retrying
    DEBUG("client connected");

    std::unique_lock<std::mutex> lock(m_user_mutex);
    using namespace std::placeholders;
    auto conn = std::make_shared<NetworkConnection>(
        std::move(stream), m_notifier,
        std::bind(&Dispatcher::ClientHandshake, this, _1, _2, _3),
        std::bind(&Storage::GetEntryType, &m_storage, _1));
    conn->set_process_incoming(
        std::bind(&Storage::ProcessIncoming, &m_storage, _1, _2,
                  std::weak_ptr<NetworkConnection>(conn)));
    m_connections.resize(0);  // disconnect any current
    m_connections.emplace_back(conn);
    conn->set_proto_rev(m_reconnect_proto_rev);
    conn->Start();

    // reconnect the next time starting with latest protocol revision
    m_reconnect_proto_rev = 0x0300;

    // block until told to reconnect
    m_do_reconnect = false;
    m_reconnect_cv.wait(lock, [&] { return !m_active || m_do_reconnect; });
  }
}

bool DispatcherBase::ClientHandshake(
    NetworkConnection& conn, std::function<std::shared_ptr<Message>()> get_msg,
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
    DEBUG4("received init str=" << msg->str() << " id=" << msg->id()
                                << " seq_num=" << msg->seq_num_uid());
    if (msg->Is(Message::kServerHelloDone)) break;
    // shouldn't receive a keep alive, but handle gracefully
    if (msg->Is(Message::kKeepAlive)) {
      msg = get_msg();
      continue;
    }
    if (!msg->Is(Message::kEntryAssign)) {
      // unexpected message
      DEBUG("client: received message ("
            << msg->type()
            << ") other than entry assignment during initial handshake");
      return false;
    }
    incoming.emplace_back(std::move(msg));
    // get the next message
    msg = get_msg();
  }

  // generate outgoing assignments
  NetworkConnection::Outgoing outgoing;

  m_storage.ApplyInitialAssignments(conn, incoming, new_server, &outgoing);

  if (conn.proto_rev() >= 0x0300)
    outgoing.emplace_back(Message::ClientHelloDone());

  if (!outgoing.empty()) send_msgs(outgoing);

  INFO("client: CONNECTED to server " << conn.stream().getPeerIP() << " port "
                                      << conn.stream().getPeerPort());
  return true;
}

bool DispatcherBase::ServerHandshake(
    NetworkConnection& conn, std::function<std::shared_ptr<Message>()> get_msg,
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
  DEBUG("server: client protocol " << proto_rev);
  conn.set_proto_rev(proto_rev);

  // Send initial set of assignments
  NetworkConnection::Outgoing outgoing;

  // Start with server hello.  TODO: initial connection flag
  if (proto_rev >= 0x0300) {
    std::lock_guard<std::mutex> lock(m_user_mutex);
    outgoing.emplace_back(Message::ServerHello(0u, m_identity));
  }

  // Get snapshot of initial assignments
  m_storage.GetInitialAssignments(conn, &outgoing);

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
      // shouldn't receive a keep alive, but handle gracefully
      if (msg->Is(Message::kKeepAlive)) {
        msg = get_msg();
        continue;
      }
      if (!msg->Is(Message::kEntryAssign)) {
        // unexpected message
        DEBUG("server: received message ("
              << msg->type()
              << ") other than entry assignment during initial handshake");
        return false;
      }
      incoming.push_back(msg);
      // get the next message (blocks)
      msg = get_msg();
    }
    for (auto& msg : incoming)
      m_storage.ProcessIncoming(msg, &conn, std::weak_ptr<NetworkConnection>());
  }

  INFO("server: client CONNECTED: " << conn.stream().getPeerIP() << " port "
                                    << conn.stream().getPeerPort());
  return true;
}

void DispatcherBase::ClientReconnect(unsigned int proto_rev) {
  if (m_server) return;
  {
    std::lock_guard<std::mutex> lock(m_user_mutex);
    m_reconnect_proto_rev = proto_rev;
    m_do_reconnect = true;
  }
  m_reconnect_cv.notify_one();
}
