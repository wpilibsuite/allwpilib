// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "NetworkServer.h"

#include <stdint.h>

#include <atomic>
#include <memory>
#include <span>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include <wpi/MemoryBuffer.h>
#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpi/fs.h>
#include <wpi/mutex.h>
#include <wpi/raw_ostream.h>
#include <wpi/timestamp.h>
#include <wpinet/HttpUtil.h>
#include <wpinet/HttpWebSocketServerConnection.h>
#include <wpinet/UrlParser.h>
#include <wpinet/uv/Tcp.h>
#include <wpinet/uv/Work.h>
#include <wpinet/uv/util.h>

#include "IConnectionList.h"
#include "InstanceImpl.h"
#include "Log.h"
#include "net/WebSocketConnection.h"
#include "net/WireDecoder.h"
#include "net/WireEncoder.h"
#include "net3/UvStreamConnection3.h"

using namespace nt;
namespace uv = wpi::uv;

// use a larger max message size for websockets
static constexpr size_t kMaxMessageSize = 2 * 1024 * 1024;

static constexpr size_t kClientProcessMessageCountMax = 16;

class NetworkServer::ServerConnection {
 public:
  ServerConnection(NetworkServer& server, std::string_view addr,
                   unsigned int port, wpi::Logger& logger)
      : m_server{server},
        m_connInfo{fmt::format("{}:{}", addr, port)},
        m_logger{logger} {
    m_info.remote_ip = addr;
    m_info.remote_port = port;
  }

  int GetClientId() const { return m_clientId; }

 protected:
  void SetupOutgoingTimer();
  void UpdateOutgoingTimer(uint32_t repeatMs);
  void ConnectionClosed();

  NetworkServer& m_server;
  ConnectionInfo m_info;
  std::string m_connInfo;
  wpi::Logger& m_logger;
  int m_clientId;

 private:
  std::shared_ptr<uv::Timer> m_outgoingTimer;
};

class NetworkServer::ServerConnection3 : public ServerConnection {
 public:
  ServerConnection3(std::shared_ptr<uv::Stream> stream, NetworkServer& server,
                    std::string_view addr, unsigned int port,
                    wpi::Logger& logger);

 private:
  std::shared_ptr<net3::UvStreamConnection3> m_wire;
};

class NetworkServer::ServerConnection4 final
    : public ServerConnection,
      public wpi::HttpWebSocketServerConnection<ServerConnection4> {
 public:
  ServerConnection4(std::shared_ptr<uv::Stream> stream, NetworkServer& server,
                    std::string_view addr, unsigned int port,
                    wpi::Logger& logger)
      : ServerConnection{server, addr, port, logger},
        HttpWebSocketServerConnection(
            stream,
            {"v4.1.networktables.first.wpi.edu", "networktables.first.wpi.edu",
             "rtt.networktables.first.wpi.edu"}) {
    m_info.protocol_version = 0x0400;
  }

 private:
  void ProcessRequest() final;
  void ProcessWsUpgrade() final;

  std::shared_ptr<net::WebSocketConnection> m_wire;
};

void NetworkServer::ServerConnection::SetupOutgoingTimer() {
  m_outgoingTimer = uv::Timer::Create(m_server.m_loop);
  m_outgoingTimer->timeout.connect([this] {
    m_server.m_serverImpl.SendOutgoing(m_clientId,
                                       m_server.m_loop.Now().count());
  });
}

void NetworkServer::ServerConnection::UpdateOutgoingTimer(uint32_t repeatMs) {
  DEBUG4("Setting periodic timer to {}", repeatMs);
  if (repeatMs == UINT32_MAX) {
    m_outgoingTimer->Stop();
  } else if (!m_outgoingTimer->IsActive() ||
             uv::Timer::Time{repeatMs} != m_outgoingTimer->GetRepeat()) {
    m_outgoingTimer->Start(uv::Timer::Time{repeatMs},
                           uv::Timer::Time{repeatMs});
  }
}

void NetworkServer::ServerConnection::ConnectionClosed() {
  // don't call back into m_server if it's being destroyed
  if (!m_outgoingTimer->IsLoopClosing()) {
    uv::Timer::SingleShot(m_outgoingTimer->GetLoopRef(), uv::Timer::Time{0},
                          [client = m_server.m_serverImpl.RemoveClient(
                               m_clientId)]() mutable { client.reset(); });
    m_server.RemoveConnection(this);
  }
  m_outgoingTimer->Close();
}

NetworkServer::ServerConnection3::ServerConnection3(
    std::shared_ptr<uv::Stream> stream, NetworkServer& server,
    std::string_view addr, unsigned int port, wpi::Logger& logger)
    : ServerConnection{server, addr, port, logger},
      m_wire{std::make_shared<net3::UvStreamConnection3>(*stream)} {
  m_info.remote_ip = addr;
  m_info.remote_port = port;

  // TODO: set local flag appropriately
  m_clientId = m_server.m_serverImpl.AddClient3(
      m_connInfo, false, *m_wire,
      [this](std::string_view name, uint16_t proto) {
        m_info.remote_id = name;
        m_info.protocol_version = proto;
        m_server.AddConnection(this, m_info);
        INFO("CONNECTED NT3 client '{}' (from {})", name, m_connInfo);
      },
      [this](uint32_t repeatMs) { UpdateOutgoingTimer(repeatMs); });

  stream->error.connect([this](uv::Error err) {
    if (!m_wire->GetDisconnectReason().empty()) {
      return;
    }
    m_wire->Disconnect(fmt::format("stream error: {}", err.name()));
    m_wire->GetStream().Shutdown([this] { m_wire->GetStream().Close(); });
  });
  stream->end.connect([this] {
    if (!m_wire->GetDisconnectReason().empty()) {
      return;
    }
    m_wire->Disconnect("remote end closed connection");
    m_wire->GetStream().Shutdown([this] { m_wire->GetStream().Close(); });
  });
  stream->closed.connect([this] {
    INFO("DISCONNECTED NT3 client '{}' (from {}): {}", m_info.remote_id,
         m_connInfo, m_wire->GetDisconnectReason());
    ConnectionClosed();
  });
  stream->data.connect([this](uv::Buffer& buf, size_t size) {
    if (m_server.m_serverImpl.ProcessIncomingBinary(
            m_clientId, {reinterpret_cast<const uint8_t*>(buf.base), size})) {
      m_server.m_idle->Start();
    }
  });
  stream->StartRead();

  SetupOutgoingTimer();
}

void NetworkServer::ServerConnection4::ProcessRequest() {
  DEBUG1("HTTP request: '{}'", m_request.GetUrl());
  wpi::UrlParser url{m_request.GetUrl(),
                     m_request.GetMethod() == wpi::HTTP_CONNECT};
  if (!url.IsValid()) {
    // failed to parse URL
    SendError(400);
    return;
  }

  std::string_view path;
  if (url.HasPath()) {
    path = url.GetPath();
  }
  DEBUG4("path: \"{}\"", path);

  std::string_view query;
  if (url.HasQuery()) {
    query = url.GetQuery();
  }
  DEBUG4("query: \"{}\"\n", query);

  const bool isGET = m_request.GetMethod() == wpi::HTTP_GET;
  if (isGET && path == "/") {
    // build HTML root page
    SendResponse(200, "OK", "text/html",
                 "<html><head><title>NetworkTables</title></head>"
                 "<body><p>WebSockets must be used to access NetworkTables."
                 "</body></html>");
  } else if (isGET && path == "/nt/persistent.json") {
    SendResponse(200, "OK", "application/json",
                 m_server.m_serverImpl.DumpPersistent());
  } else {
    SendError(404, "Resource not found");
  }
}

void NetworkServer::ServerConnection4::ProcessWsUpgrade() {
  // get name from URL
  wpi::UrlParser url{m_request.GetUrl(), false};
  std::string_view path;
  if (url.HasPath()) {
    path = url.GetPath();
  }
  DEBUG4("path: '{}'", path);

  wpi::SmallString<128> nameBuf;
  std::string_view name;
  bool err = false;
  if (auto uri = wpi::remove_prefix(path, "/nt/")) {
    name = wpi::UnescapeURI(*uri, nameBuf, &err);
  }
  if (err || name.empty()) {
    INFO("invalid path '{}' (from {}), must match /nt/[clientId], closing",
         path, m_connInfo);
    m_websocket->Fail(
        404, fmt::format("invalid path '{}', must match /nt/[clientId]", path));
    return;
  }

  m_websocket->SetMaxMessageSize(kMaxMessageSize);

  m_websocket->open.connect([this, name = std::string{name}](
                                std::string_view protocol) {
    m_info.protocol_version =
        protocol == "v4.1.networktables.first.wpi.edu" ? 0x0401 : 0x0400;
    m_wire = std::make_shared<net::WebSocketConnection>(
        *m_websocket, m_info.protocol_version, m_logger);

    if (protocol == "rtt.networktables.first.wpi.edu") {
      INFO("CONNECTED RTT client (from {})", m_connInfo);
      m_websocket->binary.connect([this](std::span<const uint8_t> data, bool) {
        while (!data.empty()) {
          // decode message
          int pubuid;
          Value value;
          std::string error;
          if (!net::WireDecodeBinary(&data, &pubuid, &value, &error, 0)) {
            m_wire->Disconnect(fmt::format("binary decode error: {}", error));
            break;
          }

          // respond to RTT ping
          if (pubuid == -1) {
            m_wire->SendBinary([&](auto& os) {
              net::WireEncodeBinary(os, -1, wpi::Now(), value);
            });
          }
        }
      });
      m_websocket->closed.connect([this](uint16_t, std::string_view reason) {
        auto realReason = m_wire->GetDisconnectReason();
        INFO("DISCONNECTED RTT client (from {}): {}", m_connInfo,
             realReason.empty() ? reason : realReason);
      });
      return;
    }

    // TODO: set local flag appropriately
    std::string dedupName;
    std::tie(dedupName, m_clientId) = m_server.m_serverImpl.AddClient(
        name, m_connInfo, false, *m_wire,
        [this](uint32_t repeatMs) { UpdateOutgoingTimer(repeatMs); });
    INFO("CONNECTED NT4 client '{}' (from {})", dedupName, m_connInfo);
    m_info.remote_id = dedupName;
    m_server.AddConnection(this, m_info);
    m_websocket->closed.connect([this](uint16_t, std::string_view reason) {
      auto realReason = m_wire->GetDisconnectReason();
      INFO("DISCONNECTED NT4 client '{}' (from {}): {}", m_info.remote_id,
           m_connInfo, realReason.empty() ? reason : realReason);
      ConnectionClosed();
    });
    m_websocket->text.connect([this](std::string_view data, bool) {
      if (m_server.m_serverImpl.ProcessIncomingText(m_clientId, data)) {
        m_server.m_idle->Start();
      }
    });
    m_websocket->binary.connect([this](std::span<const uint8_t> data, bool) {
      if (m_server.m_serverImpl.ProcessIncomingBinary(m_clientId, data)) {
        m_server.m_idle->Start();
      }
    });

    SetupOutgoingTimer();
  });
}

NetworkServer::NetworkServer(std::string_view persistentFilename,
                             std::string_view listenAddress, unsigned int port3,
                             unsigned int port4,
                             net::ILocalStorage& localStorage,
                             IConnectionList& connList, wpi::Logger& logger,
                             std::function<void()> initDone)
    : m_localStorage{localStorage},
      m_connList{connList},
      m_logger{logger},
      m_initDone{std::move(initDone)},
      m_persistentFilename{persistentFilename},
      m_listenAddress{wpi::trim(listenAddress)},
      m_port3{port3},
      m_port4{port4},
      m_serverImpl{logger},
      m_localQueue{logger},
      m_loop(*m_loopRunner.GetLoop()) {
  m_loopRunner.ExecAsync([=, this](uv::Loop& loop) {
    // connect local storage to server
    m_serverImpl.SetLocal(&m_localStorage, &m_localQueue);
    m_localStorage.StartNetwork(&m_localQueue);
    ProcessAllLocal();

    // load persistent file first, then initialize
    uv::QueueWork(m_loop, [this] { LoadPersistent(); }, [this] { Init(); });
  });
}

NetworkServer::~NetworkServer() {
  m_loopRunner.ExecAsync([this](uv::Loop&) { m_shutdown = true; });
  m_localStorage.ClearNetwork();
  m_connList.ClearConnections();
}

void NetworkServer::FlushLocal() {
  if (auto async = m_flushLocalAtomic.load(std::memory_order_relaxed)) {
    async->UnsafeSend();
  }
}

void NetworkServer::Flush() {
  if (auto async = m_flushAtomic.load(std::memory_order_relaxed)) {
    async->UnsafeSend();
  }
}

void NetworkServer::ProcessAllLocal() {
  while (m_serverImpl.ProcessLocalMessages(128)) {
  }
}

void NetworkServer::LoadPersistent() {
  auto fileBuffer = wpi::MemoryBuffer::GetFile(m_persistentFilename);
  if (!fileBuffer) {
    INFO(
        "could not open persistent file '{}': {} "
        "(this can be ignored if you aren't expecting persistent values)",
        m_persistentFilename, fileBuffer.error().message());
    // backup file
    std::error_code ec;
    fs::copy_file(m_persistentFilename, m_persistentFilename + ".bak",
                  std::filesystem::copy_options::overwrite_existing, ec);
    // try to write an empty file so it doesn't happen again
    wpi::raw_fd_ostream os{m_persistentFilename, ec, fs::F_Text};
    if (ec.value() == 0) {
      os << "[]\n";
      os.close();
    }
    return;
  }
  m_persistentData =
      std::string{fileBuffer.value()->begin(), fileBuffer.value()->end()};
  DEBUG4("read data: {}", m_persistentData);
}

void NetworkServer::SavePersistent(std::string_view filename,
                                   std::string_view data) {
  // write to temporary file
  auto tmp = fmt::format("{}.tmp", filename);
  std::error_code ec;
  wpi::raw_fd_ostream os{tmp, ec, fs::F_Text};
  if (ec.value() != 0) {
    INFO("could not open persistent file '{}' for write: {}", tmp,
         ec.message());
    return;
  }
  os << data;
  os.close();
  if (os.has_error()) {
    fs::remove(tmp);
    return;
  }

  // move to real file
  auto bak = fmt::format("{}.bck", filename);
  fs::remove(bak, ec);
  fs::rename(filename, bak, ec);
  fs::rename(tmp, filename, ec);
  if (ec.value() != 0) {
    // attempt to restore backup
    fs::rename(bak, filename, ec);
  }
}

void NetworkServer::Init() {
  if (m_shutdown) {
    return;
  }
  auto errs = m_serverImpl.LoadPersistent(m_persistentData);
  if (!errs.empty()) {
    WARN("error reading persistent file: {}", errs);
  }

  // set up timers
  m_readLocalTimer = uv::Timer::Create(m_loop);
  if (m_readLocalTimer) {
    m_readLocalTimer->timeout.connect([this] {
      if (m_serverImpl.ProcessLocalMessages(kClientProcessMessageCountMax)) {
        DEBUG4("Starting idle processing");
        m_idle->Start();  // more to process
      }
    });
    m_readLocalTimer->Start(uv::Timer::Time{100}, uv::Timer::Time{100});
  }

  m_savePersistentTimer = uv::Timer::Create(m_loop);
  if (m_savePersistentTimer) {
    m_savePersistentTimer->timeout.connect([this] {
      if (m_serverImpl.PersistentChanged()) {
        uv::QueueWork(
            m_loop,
            [this, fn = m_persistentFilename,
             data = m_serverImpl.DumpPersistent()] {
              SavePersistent(fn, data);
            },
            nullptr);
      }
    });
    m_savePersistentTimer->Start(uv::Timer::Time{1000}, uv::Timer::Time{1000});
  }

  // set up flush async
  m_flush = uv::Async<>::Create(m_loop);
  if (m_flush) {
    m_flush->wakeup.connect([this] {
      ProcessAllLocal();
      m_serverImpl.SendAllOutgoing(m_loop.Now().count(), true);
    });
  }
  m_flushAtomic = m_flush.get();

  m_flushLocal = uv::Async<>::Create(m_loop);
  if (m_flushLocal) {
    m_flushLocal->wakeup.connect([this] {
      if (m_serverImpl.ProcessLocalMessages(kClientProcessMessageCountMax)) {
        DEBUG4("Starting idle processing");
        m_idle->Start();  // more to process
      }
    });
  }
  m_flushLocalAtomic = m_flushLocal.get();

  m_idle = uv::Idle::Create(m_loop);
  if (m_idle) {
    m_idle->idle.connect([this] {
      if (m_serverImpl.ProcessIncomingMessages(kClientProcessMessageCountMax)) {
        DEBUG4("Starting idle processing");
        m_idle->Start();  // more to process
      } else {
        DEBUG4("Stopping idle processing");
        m_idle->Stop();  // go back to sleep
      }
    });
  }

  INFO("Listening on NT3 port {}, NT4 port {}", m_port3, m_port4);

  if (m_port3 != 0) {
    auto tcp3 = uv::Tcp::Create(m_loop);
    tcp3->error.connect([logger = &m_logger](uv::Error err) {
      WPI_INFO(*logger, "NT3 server socket error: {}", err.str());
    });
    tcp3->Bind(m_listenAddress, m_port3);

    // when we get a NT3 connection, accept it and start reading
    tcp3->connection.connect([this, srv = tcp3.get()] {
      auto tcp = srv->Accept();
      if (!tcp) {
        return;
      }
      tcp->error.connect([logger = &m_logger](uv::Error err) {
        WPI_INFO(*logger, "NT3 socket error: {}", err.str());
      });
      tcp->SetNoDelay(true);
      std::string peerAddr;
      unsigned int peerPort = 0;
      if (uv::AddrToName(tcp->GetPeer(), &peerAddr, &peerPort) == 0) {
        INFO("Got a NT3 connection from {} port {}", peerAddr, peerPort);
      } else {
        INFO("Got a NT3 connection from unknown");
      }
      auto conn = std::make_shared<ServerConnection3>(tcp, *this, peerAddr,
                                                      peerPort, m_logger);
      tcp->SetData(conn);
    });

    tcp3->Listen();
  }

  if (m_port4 != 0) {
    auto tcp4 = uv::Tcp::Create(m_loop);
    tcp4->error.connect([logger = &m_logger](uv::Error err) {
      WPI_INFO(*logger, "NT4 server socket error: {}", err.str());
    });
    tcp4->Bind(m_listenAddress, m_port4);

    // when we get a NT4 connection, accept it and start reading
    tcp4->connection.connect([this, srv = tcp4.get()] {
      auto tcp = srv->Accept();
      if (!tcp) {
        return;
      }
      tcp->SetLogger(&m_logger);
      tcp->error.connect([logger = &m_logger](uv::Error err) {
        WPI_INFO(*logger, "NT4 socket error: {}", err.str());
      });
      tcp->SetNoDelay(true);
      std::string peerAddr;
      unsigned int peerPort = 0;
      if (uv::AddrToName(tcp->GetPeer(), &peerAddr, &peerPort) == 0) {
        INFO("Got a NT4 connection from {} port {}", peerAddr, peerPort);
      } else {
        INFO("Got a NT4 connection from unknown");
      }
      auto conn = std::make_shared<ServerConnection4>(tcp, *this, peerAddr,
                                                      peerPort, m_logger);
      tcp->SetData(conn);
    });

    tcp4->Listen();
  }

  if (m_initDone) {
    DEBUG4("NetworkServer initDone()");
    m_initDone();
    m_initDone = nullptr;
  }
}

void NetworkServer::AddConnection(ServerConnection* conn,
                                  const ConnectionInfo& info) {
  std::scoped_lock lock{m_mutex};
  m_connections.emplace_back(Connection{conn, m_connList.AddConnection(info)});
  m_serverImpl.ConnectionsChanged(m_connList.GetConnections());
}

void NetworkServer::RemoveConnection(ServerConnection* conn) {
  std::scoped_lock lock{m_mutex};
  auto it = std::find_if(m_connections.begin(), m_connections.end(),
                         [=](auto&& c) { return c.conn == conn; });
  if (it != m_connections.end()) {
    m_connList.RemoveConnection(it->connHandle);
    m_connections.erase(it);
    m_serverImpl.ConnectionsChanged(m_connList.GetConnections());
  }
}
