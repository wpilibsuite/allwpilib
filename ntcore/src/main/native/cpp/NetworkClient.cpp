// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "NetworkClient.h"

#include <stdint.h>

#include <atomic>
#include <memory>
#include <vector>

#include <fmt/format.h>
#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpinet/DsClient.h>
#include <wpinet/EventLoopRunner.h>
#include <wpinet/HttpUtil.h>
#include <wpinet/ParallelTcpConnector.h>
#include <wpinet/WebSocket.h>
#include <wpinet/uv/Async.h>
#include <wpinet/uv/Loop.h>
#include <wpinet/uv/Tcp.h>
#include <wpinet/uv/Timer.h>
#include <wpinet/uv/util.h>

#include "IConnectionList.h"
#include "Log.h"
#include "net/ClientImpl.h"
#include "net/Message.h"
#include "net/NetworkLoopQueue.h"
#include "net/WebSocketConnection.h"
#include "net3/ClientImpl3.h"
#include "net3/UvStreamConnection3.h"

using namespace nt;
namespace uv = wpi::uv;

static constexpr uv::Timer::Time kReconnectRate{1000};
static constexpr uv::Timer::Time kWebsocketHandshakeTimeout{500};
// use a larger max message size for websockets
static constexpr size_t kMaxMessageSize = 2 * 1024 * 1024;

namespace {

class NCImpl {
 public:
  NCImpl(int inst, std::string_view id, net::ILocalStorage& localStorage,
         IConnectionList& connList, wpi::Logger& logger);
  virtual ~NCImpl() = default;

  // user-facing functions
  void SetServers(std::span<const std::pair<std::string, unsigned int>> servers,
                  unsigned int defaultPort);
  void StartDSClient(unsigned int port);
  void StopDSClient();

  virtual void TcpConnected(uv::Tcp& tcp) = 0;
  virtual void Disconnect(std::string_view reason);

  // invariants
  int m_inst;
  net::ILocalStorage& m_localStorage;
  IConnectionList& m_connList;
  wpi::Logger& m_logger;
  std::string m_id;

  // used only from loop
  std::shared_ptr<wpi::ParallelTcpConnector> m_parallelConnect;
  std::shared_ptr<uv::Timer> m_readLocalTimer;
  std::shared_ptr<uv::Timer> m_sendValuesTimer;
  std::shared_ptr<uv::Async<>> m_flushLocal;
  std::shared_ptr<uv::Async<>> m_flush;

  std::vector<net::ClientMessage> m_localMsgs;

  std::vector<std::pair<std::string, unsigned int>> m_servers;

  std::pair<std::string, unsigned int> m_dsClientServer{"", 0};
  std::shared_ptr<wpi::DsClient> m_dsClient;

  // shared with user
  std::atomic<uv::Async<>*> m_flushLocalAtomic{nullptr};
  std::atomic<uv::Async<>*> m_flushAtomic{nullptr};

  net::NetworkLoopQueue m_localQueue;

  int m_connHandle = 0;

  wpi::EventLoopRunner m_loopRunner;
  uv::Loop& m_loop;
};

class NCImpl3 : public NCImpl {
 public:
  NCImpl3(int inst, std::string_view id, net::ILocalStorage& localStorage,
          IConnectionList& connList, wpi::Logger& logger);
  ~NCImpl3() override;

  void HandleLocal();
  void TcpConnected(uv::Tcp& tcp) final;
  void Disconnect(std::string_view reason) override;

  std::shared_ptr<net3::UvStreamConnection3> m_wire;
  std::shared_ptr<net3::ClientImpl3> m_clientImpl;
};

class NCImpl4 : public NCImpl {
 public:
  NCImpl4(int inst, std::string_view id, net::ILocalStorage& localStorage,
          IConnectionList& connList, wpi::Logger& logger);
  ~NCImpl4() override;

  void HandleLocal();
  void TcpConnected(uv::Tcp& tcp) final;
  void WsConnected(wpi::WebSocket& ws, uv::Tcp& tcp);
  void Disconnect(std::string_view reason) override;

  std::shared_ptr<net::WebSocketConnection> m_wire;
  std::unique_ptr<net::ClientImpl> m_clientImpl;
};

}  // namespace

NCImpl::NCImpl(int inst, std::string_view id, net::ILocalStorage& localStorage,
               IConnectionList& connList, wpi::Logger& logger)
    : m_inst{inst},
      m_localStorage{localStorage},
      m_connList{connList},
      m_logger{logger},
      m_id{id},
      m_localQueue{logger},
      m_loop{*m_loopRunner.GetLoop()} {
  m_localMsgs.reserve(net::NetworkLoopQueue::kInitialQueueSize);

  INFO("starting network client");
}

void NCImpl::SetServers(
    std::span<const std::pair<std::string, unsigned int>> servers,
    unsigned int defaultPort) {
  std::vector<std::pair<std::string, unsigned int>> serversCopy;
  serversCopy.reserve(servers.size());
  for (auto&& server : servers) {
    serversCopy.emplace_back(wpi::trim(server.first),
                             server.second == 0 ? defaultPort : server.second);
  }

  m_loopRunner.ExecAsync(
      [this, servers = std::move(serversCopy)](uv::Loop&) mutable {
        m_servers = std::move(servers);
        if (m_dsClientServer.first.empty()) {
          m_parallelConnect->SetServers(m_servers);
        }
      });
}

void NCImpl::StartDSClient(unsigned int port) {
  m_loopRunner.ExecAsync([=, this](uv::Loop& loop) {
    if (m_dsClient) {
      return;
    }
    m_dsClientServer.second = port == 0 ? NT_DEFAULT_PORT4 : port;
    m_dsClient = wpi::DsClient::Create(m_loop, m_logger);
    m_dsClient->setIp.connect([this](std::string_view ip) {
      m_dsClientServer.first = ip;
      m_parallelConnect->SetServers({{m_dsClientServer}});
    });
    m_dsClient->clearIp.connect([this] {
      m_dsClientServer.first.clear();
      m_parallelConnect->SetServers(m_servers);
    });
  });
}

void NCImpl::StopDSClient() {
  m_loopRunner.ExecAsync([this](uv::Loop& loop) {
    if (m_dsClient) {
      m_dsClient->Close();
      m_dsClient.reset();
    }
  });
}

void NCImpl::Disconnect(std::string_view reason) {
  if (m_readLocalTimer) {
    m_readLocalTimer->Stop();
  }
  m_sendValuesTimer->Stop();
  m_localStorage.ClearNetwork();
  m_localQueue.ClearQueue();
  m_connList.RemoveConnection(m_connHandle);
  m_connHandle = 0;

  // start trying to connect again
  uv::Timer::SingleShot(m_loop, kReconnectRate,
                        [this] { m_parallelConnect->Disconnected(); });
}

NCImpl3::NCImpl3(int inst, std::string_view id,
                 net::ILocalStorage& localStorage, IConnectionList& connList,
                 wpi::Logger& logger)
    : NCImpl{inst, id, localStorage, connList, logger} {
  m_loopRunner.ExecAsync([this](uv::Loop& loop) {
    m_parallelConnect = wpi::ParallelTcpConnector::Create(
        loop, kReconnectRate, m_logger,
        [this](uv::Tcp& tcp) { TcpConnected(tcp); });

    m_sendValuesTimer = uv::Timer::Create(loop);
    m_sendValuesTimer->timeout.connect([this] {
      if (m_clientImpl) {
        HandleLocal();
        m_clientImpl->SendPeriodic(m_loop.Now().count());
      }
    });

    // set up flush async
    m_flush = uv::Async<>::Create(m_loop);
    m_flush->wakeup.connect([this] {
      if (m_clientImpl) {
        HandleLocal();
        m_clientImpl->SendPeriodic(m_loop.Now().count());
      }
    });
    m_flushAtomic = m_flush.get();

    m_flushLocal = uv::Async<>::Create(m_loop);
    m_flushLocal->wakeup.connect([this] { HandleLocal(); });
    m_flushLocalAtomic = m_flushLocal.get();
  });
}

NCImpl3::~NCImpl3() {
  // must explicitly destroy these on loop
  m_loopRunner.ExecSync([&](auto&) {
    m_clientImpl.reset();
    m_wire.reset();
  });
  // shut down loop here to avoid race
  m_loopRunner.Stop();
}

void NCImpl3::HandleLocal() {
  m_localQueue.ReadQueue(&m_localMsgs);
  if (m_clientImpl) {
    m_clientImpl->HandleLocal(m_localMsgs);
  }
}

void NCImpl3::TcpConnected(uv::Tcp& tcp) {
  tcp.SetNoDelay(true);

  // create as shared_ptr and capture in lambda because there may be multiple
  // simultaneous attempts
  auto wire = std::make_shared<net3::UvStreamConnection3>(tcp);
  auto clientImpl = std::make_shared<net3::ClientImpl3>(
      m_loop.Now().count(), m_inst, *wire, m_logger, [this](uint32_t repeatMs) {
        DEBUG4("Setting periodic timer to {}", repeatMs);
        m_sendValuesTimer->Start(uv::Timer::Time{repeatMs},
                                 uv::Timer::Time{repeatMs});
      });
  clientImpl->Start(
      m_id, [this, wire,
             clientWeak = std::weak_ptr<net3::ClientImpl3>{clientImpl}, &tcp] {
        auto clientImpl = clientWeak.lock();
        if (!clientImpl) {
          return;
        }
        if (m_connList.IsConnected()) {
          tcp.Close();  // no longer needed
          return;
        }

        m_parallelConnect->Succeeded(tcp);

        m_wire = std::move(wire);
        m_clientImpl = std::move(clientImpl);

        ConnectionInfo connInfo;
        uv::AddrToName(tcp.GetPeer(), &connInfo.remote_ip,
                       &connInfo.remote_port);
        connInfo.protocol_version = 0x0300;

        INFO("CONNECTED NT3 to {} port {}", connInfo.remote_ip,
             connInfo.remote_port);
        m_connHandle = m_connList.AddConnection(connInfo);

        tcp.error.connect([this, &tcp](uv::Error err) {
          DEBUG3("NT3 TCP error {}", err.str());
          if (!tcp.IsLoopClosing()) {
            Disconnect(err.str());
          }
        });
        tcp.end.connect([this, &tcp] {
          DEBUG3("NT3 TCP read ended");
          if (!tcp.IsLoopClosing()) {
            Disconnect("remote end closed connection");
          }
        });
        tcp.closed.connect([this, &tcp] {
          DEBUG3("NT3 TCP connection closed");
          if (!tcp.IsLoopClosing()) {
            Disconnect(m_wire->GetDisconnectReason());
          }
        });

        {
          net3::ClientStartup3 startup{*m_clientImpl};
          m_localStorage.StartNetwork(startup, &m_localQueue);
        }
        m_clientImpl->SetLocal(&m_localStorage);
      });

  tcp.SetData(clientImpl);
  tcp.data.connect(
      [clientImpl = clientImpl.get()](uv::Buffer& buf, size_t len) {
        clientImpl->ProcessIncoming(
            {reinterpret_cast<const uint8_t*>(buf.base), len});
      });
  tcp.StartRead();
}

void NCImpl3::Disconnect(std::string_view reason) {
  INFO("DISCONNECTED NT3 connection: {}", reason);
  m_clientImpl.reset();
  m_wire.reset();
  NCImpl::Disconnect(reason);
}

NCImpl4::NCImpl4(int inst, std::string_view id,
                 net::ILocalStorage& localStorage, IConnectionList& connList,
                 wpi::Logger& logger)
    : NCImpl{inst, id, localStorage, connList, logger} {
  m_loopRunner.ExecAsync([this](uv::Loop& loop) {
    m_parallelConnect = wpi::ParallelTcpConnector::Create(
        loop, kReconnectRate, m_logger,
        [this](uv::Tcp& tcp) { TcpConnected(tcp); });

    m_readLocalTimer = uv::Timer::Create(loop);
    m_readLocalTimer->timeout.connect([this] {
      if (m_clientImpl) {
        HandleLocal();
        m_clientImpl->SendControl(m_loop.Now().count());
      }
    });
    m_readLocalTimer->Start(uv::Timer::Time{100}, uv::Timer::Time{100});

    m_sendValuesTimer = uv::Timer::Create(loop);
    m_sendValuesTimer->timeout.connect([this] {
      if (m_clientImpl) {
        HandleLocal();
        m_clientImpl->SendValues(m_loop.Now().count());
      }
    });

    // set up flush async
    m_flush = uv::Async<>::Create(m_loop);
    m_flush->wakeup.connect([this] {
      if (m_clientImpl) {
        HandleLocal();
        m_clientImpl->SendValues(m_loop.Now().count());
      }
    });
    m_flushAtomic = m_flush.get();

    m_flushLocal = uv::Async<>::Create(m_loop);
    m_flushLocal->wakeup.connect([this] { HandleLocal(); });
    m_flushLocalAtomic = m_flushLocal.get();
  });
}

NCImpl4::~NCImpl4() {
  // must explicitly destroy these on loop
  m_loopRunner.ExecSync([&](auto&) {
    m_clientImpl.reset();
    m_wire.reset();
  });
  // shut down loop here to avoid race
  m_loopRunner.Stop();
}

void NCImpl4::HandleLocal() {
  m_localQueue.ReadQueue(&m_localMsgs);
  if (m_clientImpl) {
    m_clientImpl->HandleLocal(std::move(m_localMsgs));
  }
}

void NCImpl4::TcpConnected(uv::Tcp& tcp) {
  tcp.SetNoDelay(true);
  // Start the WS client
  if (m_logger.min_level() >= wpi::WPI_LOG_DEBUG4) {
    std::string ip;
    unsigned int port = 0;
    uv::AddrToName(tcp.GetPeer(), &ip, &port);
    DEBUG4("Starting WebSocket client on {} port {}", ip, port);
  }
  wpi::WebSocket::ClientOptions options;
  options.handshakeTimeout = kWebsocketHandshakeTimeout;
  wpi::SmallString<128> idBuf;
  auto ws = wpi::WebSocket::CreateClient(
      tcp, fmt::format("/nt/{}", wpi::EscapeURI(m_id, idBuf)), "",
      {{"networktables.first.wpi.edu"}}, options);
  ws->SetMaxMessageSize(kMaxMessageSize);
  ws->open.connect([this, &tcp, ws = ws.get()](std::string_view) {
    if (m_connList.IsConnected()) {
      ws->Terminate(1006, "no longer needed");
      return;
    }
    WsConnected(*ws, tcp);
  });
}

void NCImpl4::WsConnected(wpi::WebSocket& ws, uv::Tcp& tcp) {
  m_parallelConnect->Succeeded(tcp);

  ConnectionInfo connInfo;
  uv::AddrToName(tcp.GetPeer(), &connInfo.remote_ip, &connInfo.remote_port);
  connInfo.protocol_version = 0x0400;

  INFO("CONNECTED NT4 to {} port {}", connInfo.remote_ip, connInfo.remote_port);
  m_connHandle = m_connList.AddConnection(connInfo);

  m_wire = std::make_shared<net::WebSocketConnection>(ws);
  m_clientImpl = std::make_unique<net::ClientImpl>(
      m_loop.Now().count(), m_inst, *m_wire, m_logger,
      [this](uint32_t repeatMs) {
        DEBUG4("Setting periodic timer to {}", repeatMs);
        m_sendValuesTimer->Start(uv::Timer::Time{repeatMs},
                                 uv::Timer::Time{repeatMs});
      });
  {
    net::ClientStartup startup{*m_clientImpl};
    m_localStorage.StartNetwork(startup, &m_localQueue);
  }
  m_clientImpl->SetLocal(&m_localStorage);
  ws.closed.connect([this, &ws](uint16_t, std::string_view reason) {
    if (!ws.GetStream().IsLoopClosing()) {
      Disconnect(reason);
    }
  });
  ws.text.connect([this](std::string_view data, bool) {
    if (m_clientImpl) {
      m_clientImpl->ProcessIncomingText(data);
    }
  });
  ws.binary.connect([this](std::span<const uint8_t> data, bool) {
    if (m_clientImpl) {
      m_clientImpl->ProcessIncomingBinary(data);
    }
  });
}

void NCImpl4::Disconnect(std::string_view reason) {
  INFO("DISCONNECTED NT4 connection: {}", reason);
  m_clientImpl.reset();
  m_wire.reset();
  NCImpl::Disconnect(reason);
}

class NetworkClient::Impl final : public NCImpl4 {
 public:
  Impl(int inst, std::string_view id, net::ILocalStorage& localStorage,
       IConnectionList& connList, wpi::Logger& logger)
      : NCImpl4{inst, id, localStorage, connList, logger} {}
};

NetworkClient::NetworkClient(int inst, std::string_view id,
                             net::ILocalStorage& localStorage,
                             IConnectionList& connList, wpi::Logger& logger)
    : m_impl{std::make_unique<Impl>(inst, id, localStorage, connList, logger)} {
}

NetworkClient::~NetworkClient() {
  m_impl->m_localStorage.ClearNetwork();
  m_impl->m_connList.ClearConnections();
}

void NetworkClient::SetServers(
    std::span<const std::pair<std::string, unsigned int>> servers) {
  m_impl->SetServers(servers, NT_DEFAULT_PORT4);
}

void NetworkClient::StartDSClient(unsigned int port) {
  m_impl->StartDSClient(port);
}

void NetworkClient::StopDSClient() {
  m_impl->StopDSClient();
}

void NetworkClient::FlushLocal() {
  m_impl->m_loopRunner.ExecAsync([this](uv::Loop&) { m_impl->HandleLocal(); });
}

void NetworkClient::Flush() {
  m_impl->m_loopRunner.ExecAsync([this](uv::Loop&) {
    m_impl->HandleLocal();
    if (m_impl->m_clientImpl) {
      m_impl->m_clientImpl->SendValues(m_impl->m_loop.Now().count());
    }
  });
}

class NetworkClient3::Impl final : public NCImpl3 {
 public:
  Impl(int inst, std::string_view id, net::ILocalStorage& localStorage,
       IConnectionList& connList, wpi::Logger& logger)
      : NCImpl3{inst, id, localStorage, connList, logger} {}
};

NetworkClient3::NetworkClient3(int inst, std::string_view id,
                               net::ILocalStorage& localStorage,
                               IConnectionList& connList, wpi::Logger& logger)
    : m_impl{std::make_unique<Impl>(inst, id, localStorage, connList, logger)} {
}

NetworkClient3::~NetworkClient3() {
  m_impl->m_localStorage.ClearNetwork();
  m_impl->m_connList.ClearConnections();
}

void NetworkClient3::SetServers(
    std::span<const std::pair<std::string, unsigned int>> servers) {
  m_impl->SetServers(servers, NT_DEFAULT_PORT3);
}

void NetworkClient3::StartDSClient(unsigned int port) {
  m_impl->StartDSClient(port);
}

void NetworkClient3::StopDSClient() {
  m_impl->StopDSClient();
}

void NetworkClient3::FlushLocal() {
  if (auto async = m_impl->m_flushLocalAtomic.load(std::memory_order_relaxed)) {
    async->UnsafeSend();
  }
}

void NetworkClient3::Flush() {
  if (auto async = m_impl->m_flushAtomic.load(std::memory_order_relaxed)) {
    async->UnsafeSend();
  }
}
