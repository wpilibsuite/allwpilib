// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "NetworkServer.h"

#include <stdint.h>

#include <atomic>
#include <span>
#include <system_error>
#include <vector>

#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpi/fs.h>
#include <wpi/mutex.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>
#include <wpinet/EventLoopRunner.h>
#include <wpinet/HttpUtil.h>
#include <wpinet/HttpWebSocketServerConnection.h>
#include <wpinet/UrlParser.h>
#include <wpinet/uv/Async.h>
#include <wpinet/uv/Tcp.h>
#include <wpinet/uv/Work.h>
#include <wpinet/uv/util.h>

#include "IConnectionList.h"
#include "InstanceImpl.h"
#include "Log.h"
#include "net/Message.h"
#include "net/NetworkLoopQueue.h"
#include "net/ServerImpl.h"
#include "net/WebSocketConnection.h"
#include "net3/UvStreamConnection3.h"

using namespace nt;
namespace uv = wpi::uv;

// use a larger max message size for websockets
static constexpr size_t kMaxMessageSize = 2 * 1024 * 1024;

namespace {

class NSImpl;

class ServerConnection {
 public:
  ServerConnection(NSImpl& server, std::string_view addr, unsigned int port,
                   wpi::Logger& logger)
      : m_server{server},
        m_connInfo{fmt::format("{}:{}", addr, port)},
        m_logger{logger} {
    m_info.remote_ip = addr;
    m_info.remote_port = port;
  }

  int GetClientId() const { return m_clientId; }

 protected:
  void SetupPeriodicTimer();
  void UpdatePeriodicTimer(uint32_t repeatMs);
  void ConnectionClosed();

  NSImpl& m_server;
  ConnectionInfo m_info;
  std::string m_connInfo;
  wpi::Logger& m_logger;
  int m_clientId;

 private:
  std::shared_ptr<uv::Timer> m_sendValuesTimer;
};

class ServerConnection4 final
    : public ServerConnection,
      public wpi::HttpWebSocketServerConnection<ServerConnection4> {
 public:
  ServerConnection4(std::shared_ptr<uv::Stream> stream, NSImpl& server,
                    std::string_view addr, unsigned int port,
                    wpi::Logger& logger)
      : ServerConnection{server, addr, port, logger},
        HttpWebSocketServerConnection(stream, {"networktables.first.wpi.edu"}) {
    m_info.protocol_version = 0x0400;
  }

 private:
  void ProcessRequest() final;
  void ProcessWsUpgrade() final;

  std::shared_ptr<net::WebSocketConnection> m_wire;
};

class ServerConnection3 : public ServerConnection {
 public:
  ServerConnection3(std::shared_ptr<uv::Stream> stream, NSImpl& server,
                    std::string_view addr, unsigned int port,
                    wpi::Logger& logger);

 private:
  std::shared_ptr<net3::UvStreamConnection3> m_wire;
};

class NSImpl {
 public:
  NSImpl(std::string_view persistFilename, std::string_view listenAddress,
         unsigned int port3, unsigned int port4,
         net::ILocalStorage& localStorage, IConnectionList& connList,
         wpi::Logger& logger, std::function<void()> initDone);

  void HandleLocal();
  void LoadPersistent();
  void SavePersistent(std::string_view filename, std::string_view data);
  void Init();
  void AddConnection(ServerConnection* conn, const ConnectionInfo& info);
  void RemoveConnection(ServerConnection* conn);

  net::ILocalStorage& m_localStorage;
  IConnectionList& m_connList;
  wpi::Logger& m_logger;
  std::function<void()> m_initDone;
  std::string m_persistentData;
  std::string m_persistentFilename;
  std::string m_listenAddress;
  unsigned int m_port3;
  unsigned int m_port4;

  // used only from loop
  std::shared_ptr<uv::Timer> m_readLocalTimer;
  std::shared_ptr<uv::Timer> m_savePersistentTimer;
  std::shared_ptr<uv::Async<>> m_flushLocal;
  std::shared_ptr<uv::Async<>> m_flush;

  std::vector<net::ClientMessage> m_localMsgs;

  net::ServerImpl m_serverImpl;

  // shared with user (must be atomic or mutex-protected)
  std::atomic<uv::Async<>*> m_flushLocalAtomic{nullptr};
  std::atomic<uv::Async<>*> m_flushAtomic{nullptr};
  mutable wpi::mutex m_mutex;
  struct Connection {
    ServerConnection* conn;
    int connHandle;
  };
  std::vector<Connection> m_connections;

  net::NetworkLoopQueue m_localQueue;

  wpi::EventLoopRunner m_loopRunner;
  wpi::uv::Loop& m_loop;
};

}  // namespace

void ServerConnection::SetupPeriodicTimer() {
  m_sendValuesTimer = uv::Timer::Create(m_server.m_loop);
  m_sendValuesTimer->timeout.connect([this] {
    m_server.HandleLocal();
    m_server.m_serverImpl.SendValues(m_clientId, m_server.m_loop.Now().count());
  });
}

void ServerConnection::UpdatePeriodicTimer(uint32_t repeatMs) {
  if (repeatMs == UINT32_MAX) {
    m_sendValuesTimer->Stop();
  } else {
    m_sendValuesTimer->Start(uv::Timer::Time{repeatMs},
                             uv::Timer::Time{repeatMs});
  }
}

void ServerConnection::ConnectionClosed() {
  // don't call back into m_server if it's being destroyed
  if (!m_sendValuesTimer->IsLoopClosing()) {
    m_server.m_serverImpl.RemoveClient(m_clientId);
    m_server.RemoveConnection(this);
  }
  m_sendValuesTimer->Close();
}

void ServerConnection4::ProcessRequest() {
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

void ServerConnection4::ProcessWsUpgrade() {
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
  if (wpi::starts_with(path, "/nt/")) {
    name = wpi::UnescapeURI(wpi::drop_front(path, 4), nameBuf, &err);
  }
  if (err || name.empty()) {
    INFO("invalid path '{}' (from {}), must match /nt/[clientId], closing",
         path, m_connInfo);
    m_websocket->Fail(
        404, fmt::format("invalid path '{}', must match /nt/[clientId]", path));
    return;
  }

  m_websocket->SetMaxMessageSize(kMaxMessageSize);

  m_websocket->open.connect([this, name = std::string{name}](std::string_view) {
    m_wire = std::make_shared<net::WebSocketConnection>(*m_websocket);
    // TODO: set local flag appropriately
    std::string dedupName;
    std::tie(dedupName, m_clientId) = m_server.m_serverImpl.AddClient(
        name, m_connInfo, false, *m_wire,
        [this](uint32_t repeatMs) { UpdatePeriodicTimer(repeatMs); });
    INFO("CONNECTED NT4 client '{}' (from {})", dedupName, m_connInfo);
    m_info.remote_id = dedupName;
    m_server.AddConnection(this, m_info);
    m_websocket->closed.connect([this](uint16_t, std::string_view reason) {
      INFO("DISCONNECTED NT4 client '{}' (from {}): {}", m_info.remote_id,
           m_connInfo, reason);
      ConnectionClosed();
    });
    m_websocket->text.connect([this](std::string_view data, bool) {
      m_server.m_serverImpl.ProcessIncomingText(m_clientId, data);
    });
    m_websocket->binary.connect([this](std::span<const uint8_t> data, bool) {
      m_server.m_serverImpl.ProcessIncomingBinary(m_clientId, data);
    });

    SetupPeriodicTimer();
  });
}

ServerConnection3::ServerConnection3(std::shared_ptr<uv::Stream> stream,
                                     NSImpl& server, std::string_view addr,
                                     unsigned int port, wpi::Logger& logger)
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
      [this](uint32_t repeatMs) { UpdatePeriodicTimer(repeatMs); });

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
    m_server.m_serverImpl.ProcessIncomingBinary(
        m_clientId, {reinterpret_cast<const uint8_t*>(buf.base), size});
  });
  stream->StartRead();

  SetupPeriodicTimer();
}

NSImpl::NSImpl(std::string_view persistentFilename,
               std::string_view listenAddress, unsigned int port3,
               unsigned int port4, net::ILocalStorage& localStorage,
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
  m_localMsgs.reserve(net::NetworkLoopQueue::kInitialQueueSize);
  m_loopRunner.ExecAsync([=, this](uv::Loop& loop) {
    // connect local storage to server
    {
      net::ServerStartup startup{m_serverImpl};
      m_localStorage.StartNetwork(startup, &m_localQueue);
    }
    m_serverImpl.SetLocal(&m_localStorage);

    // load persistent file first, then initialize
    uv::QueueWork(
        m_loop, [this] { LoadPersistent(); }, [this] { Init(); });
  });
}

void NSImpl::HandleLocal() {
  m_localQueue.ReadQueue(&m_localMsgs);
  m_serverImpl.HandleLocal(m_localMsgs);
}

void NSImpl::LoadPersistent() {
  std::error_code ec;
  auto size = fs::file_size(m_persistentFilename, ec);
  wpi::raw_fd_istream is{m_persistentFilename, ec};
  if (ec.value() != 0) {
    INFO("could not open persistent file '{}': {}", m_persistentFilename,
         ec.message());
    return;
  }
  is.readinto(m_persistentData, size);
  DEBUG4("read data: {}", m_persistentData);
  if (is.has_error()) {
    WARNING("error reading persistent file");
    return;
  }
}

void NSImpl::SavePersistent(std::string_view filename, std::string_view data) {
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

void NSImpl::Init() {
  auto errs = m_serverImpl.LoadPersistent(m_persistentData);
  if (!errs.empty()) {
    WARNING("error reading persistent file: {}", errs);
  }

  // set up timers
  m_readLocalTimer = uv::Timer::Create(m_loop);
  m_readLocalTimer->timeout.connect([this] {
    HandleLocal();
    m_serverImpl.SendControl(m_loop.Now().count());
  });
  m_readLocalTimer->Start(uv::Timer::Time{100}, uv::Timer::Time{100});

  m_savePersistentTimer = uv::Timer::Create(m_loop);
  m_savePersistentTimer->timeout.connect([this] {
    if (m_serverImpl.PersistentChanged()) {
      uv::QueueWork(
          m_loop,
          [this, fn = m_persistentFilename,
           data = m_serverImpl.DumpPersistent()] { SavePersistent(fn, data); },
          nullptr);
    }
  });
  m_savePersistentTimer->Start(uv::Timer::Time{1000}, uv::Timer::Time{1000});

  // set up flush async
  m_flush = uv::Async<>::Create(m_loop);
  m_flush->wakeup.connect([this] {
    HandleLocal();
    for (auto&& conn : m_connections) {
      m_serverImpl.SendValues(conn.conn->GetClientId(), m_loop.Now().count());
    }
  });
  m_flushAtomic = m_flush.get();

  m_flushLocal = uv::Async<>::Create(m_loop);
  m_flushLocal->wakeup.connect([this] { HandleLocal(); });
  m_flushLocalAtomic = m_flushLocal.get();

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

void NSImpl::AddConnection(ServerConnection* conn, const ConnectionInfo& info) {
  std::scoped_lock lock{m_mutex};
  m_connections.emplace_back(Connection{conn, m_connList.AddConnection(info)});
  m_serverImpl.ConnectionsChanged(m_connList.GetConnections());
}

void NSImpl::RemoveConnection(ServerConnection* conn) {
  std::scoped_lock lock{m_mutex};
  auto it = std::find_if(m_connections.begin(), m_connections.end(),
                         [=](auto&& c) { return c.conn == conn; });
  if (it != m_connections.end()) {
    m_connList.RemoveConnection(it->connHandle);
    m_connections.erase(it);
    m_serverImpl.ConnectionsChanged(m_connList.GetConnections());
  }
}

class NetworkServer::Impl final : public NSImpl {
 public:
  Impl(std::string_view persistFilename, std::string_view listenAddress,
       unsigned int port3, unsigned int port4, net::ILocalStorage& localStorage,
       IConnectionList& connList, wpi::Logger& logger,
       std::function<void()> initDone)
      : NSImpl{persistFilename, listenAddress, port3,  port4,
               localStorage,    connList,      logger, std::move(initDone)} {}
};

NetworkServer::NetworkServer(std::string_view persistFilename,
                             std::string_view listenAddress, unsigned int port3,
                             unsigned int port4,
                             net::ILocalStorage& localStorage,
                             IConnectionList& connList, wpi::Logger& logger,
                             std::function<void()> initDone)
    : m_impl{std::make_unique<Impl>(persistFilename, listenAddress, port3,
                                    port4, localStorage, connList, logger,
                                    std::move(initDone))} {}

NetworkServer::~NetworkServer() {
  m_impl->m_localStorage.ClearNetwork();
  m_impl->m_connList.ClearConnections();
}

void NetworkServer::FlushLocal() {
  if (auto async = m_impl->m_flushLocalAtomic.load(std::memory_order_relaxed)) {
    async->UnsafeSend();
  }
}

void NetworkServer::Flush() {
  if (auto async = m_impl->m_flushAtomic.load(std::memory_order_relaxed)) {
    async->UnsafeSend();
  }
}
