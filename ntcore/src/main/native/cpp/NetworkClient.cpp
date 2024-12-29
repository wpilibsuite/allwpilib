// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "NetworkClient.h"

#include <stdint.h>

#include <atomic>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpinet/HttpUtil.h>
#include <wpinet/uv/Loop.h>
#include <wpinet/uv/Tcp.h>
#include <wpinet/uv/util.h>

#include "IConnectionList.h"
#include "Log.h"
#include "net/NetworkInterface.h"

using namespace nt;
namespace uv = wpi::uv;

static constexpr uv::Timer::Time kReconnectRate{1000};
static constexpr uv::Timer::Time kWebsocketHandshakeTimeout{500};
// use a larger max message size for websockets
static constexpr size_t kMaxMessageSize = 2 * 1024 * 1024;

NetworkClientBase::NetworkClientBase(int inst, std::string_view id,
                                     net::ILocalStorage& localStorage,
                                     IConnectionList& connList,
                                     wpi::Logger& logger)
    : m_inst{inst},
      m_localStorage{localStorage},
      m_connList{connList},
      m_logger{logger},
      m_id{id},
      m_localQueue{logger},
      m_loop{*m_loopRunner.GetLoop()} {
  INFO("starting network client");
}

NetworkClientBase::~NetworkClientBase() {
  m_localStorage.ClearNetwork();
  m_connList.ClearConnections();
}

void NetworkClientBase::Disconnect() {
  m_loopRunner.ExecAsync(
      [this](auto&) { ForceDisconnect("requested by application"); });
}

void NetworkClientBase::StartDSClient(unsigned int port) {
  m_loopRunner.ExecAsync([=, this](uv::Loop& loop) {
    if (m_dsClient) {
      return;
    }
    m_dsClientServer.second = port == 0 ? NT_DEFAULT_PORT4 : port;
    m_dsClient = wpi::DsClient::Create(m_loop, m_logger);
    if (m_dsClient) {
      m_dsClient->setIp.connect([this](std::string_view ip) {
        m_dsClientServer.first = ip;
        if (m_parallelConnect) {
          m_parallelConnect->SetServers({{m_dsClientServer}});
        }
      });
      m_dsClient->clearIp.connect([this] {
        m_dsClientServer.first.clear();
        if (m_parallelConnect) {
          m_parallelConnect->SetServers(m_servers);
        }
      });
    }
  });
}

void NetworkClientBase::StopDSClient() {
  m_loopRunner.ExecAsync([this](uv::Loop& loop) {
    if (m_dsClient) {
      m_dsClient->Close();
      m_dsClient.reset();
    }
  });
}

void NetworkClientBase::FlushLocal() {
  if (auto async = m_flushLocalAtomic.load(std::memory_order_relaxed)) {
    async->UnsafeSend();
  }
}

void NetworkClientBase::Flush() {
  if (auto async = m_flushAtomic.load(std::memory_order_relaxed)) {
    async->UnsafeSend();
  }
}

void NetworkClientBase::DoSetServers(
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
          if (m_parallelConnect) {
            m_parallelConnect->SetServers(m_servers);
          }
        }
      });
}

void NetworkClientBase::DoDisconnect(std::string_view reason) {
  if (m_readLocalTimer) {
    m_readLocalTimer->Stop();
  }
  if (m_sendOutgoingTimer) {
    m_sendOutgoingTimer->Stop();
  }
  m_localStorage.ClearNetwork();
  m_localQueue.ClearQueue();
  m_connList.RemoveConnection(m_connHandle);
  m_connHandle = 0;

  // start trying to connect again
  uv::Timer::SingleShot(m_loop, kReconnectRate, [this] {
    if (m_parallelConnect) {
      m_parallelConnect->Disconnected();
    }
  });
}

NetworkClient3::NetworkClient3(int inst, std::string_view id,
                               net::ILocalStorage& localStorage,
                               IConnectionList& connList, wpi::Logger& logger)
    : NetworkClientBase{inst, id, localStorage, connList, logger} {
  m_loopRunner.ExecAsync([this](uv::Loop& loop) {
    m_parallelConnect = wpi::ParallelTcpConnector::Create(
        loop, kReconnectRate, m_logger,
        [this](uv::Tcp& tcp) { TcpConnected(tcp); }, true);

    m_sendOutgoingTimer = uv::Timer::Create(loop);
    if (m_sendOutgoingTimer) {
      m_sendOutgoingTimer->timeout.connect([this] {
        if (m_clientImpl) {
          HandleLocal();
          m_clientImpl->SendPeriodic(m_loop.Now().count(), false);
        }
      });
    }

    // set up flush async
    m_flush = uv::Async<>::Create(m_loop);
    if (m_flush) {
      m_flush->wakeup.connect([this] {
        if (m_clientImpl) {
          HandleLocal();
          m_clientImpl->SendPeriodic(m_loop.Now().count(), true);
        }
      });
    }
    m_flushAtomic = m_flush.get();

    m_flushLocal = uv::Async<>::Create(m_loop);
    if (m_flushLocal) {
      m_flushLocal->wakeup.connect([this] { HandleLocal(); });
    }
    m_flushLocalAtomic = m_flushLocal.get();
  });
}

NetworkClient3::~NetworkClient3() {
  // must explicitly destroy these on loop
  m_loopRunner.ExecSync([&](auto&) {
    m_clientImpl.reset();
    m_wire.reset();
  });
  // shut down loop here to avoid race
  m_loopRunner.Stop();
}

void NetworkClient3::HandleLocal() {
  for (;;) {
    auto msgs = m_localQueue.ReadQueue(m_localMsgs);
    if (msgs.empty()) {
      return;
    }
    if (m_clientImpl) {
      m_clientImpl->HandleLocal(msgs);
    }
  }
}

void NetworkClient3::TcpConnected(uv::Tcp& tcp) {
  tcp.SetNoDelay(true);

  // create as shared_ptr and capture in lambda because there may be multiple
  // simultaneous attempts
  auto wire = std::make_shared<net3::UvStreamConnection3>(tcp);
  auto clientImpl = std::make_shared<net3::ClientImpl3>(
      m_loop.Now().count(), m_inst, *wire, m_logger, [this](uint32_t repeatMs) {
        DEBUG4("Setting periodic timer to {}", repeatMs);
        if (m_sendOutgoingTimer &&
            (!m_sendOutgoingTimer->IsActive() ||
             uv::Timer::Time{repeatMs} != m_sendOutgoingTimer->GetRepeat())) {
          m_sendOutgoingTimer->Start(uv::Timer::Time{repeatMs},
                                     uv::Timer::Time{repeatMs});
        }
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

        if (m_parallelConnect) {
          m_parallelConnect->Succeeded(tcp);
        }

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
            // we could be in the middle of sending data, so defer disconnect
            uv::Timer::SingleShot(m_loop, uv::Timer::Time{0},
                                  [this, reason = std::string{err.str()}] {
                                    DoDisconnect(reason);
                                  });
          }
        });
        tcp.end.connect([this, &tcp] {
          DEBUG3("NT3 TCP read ended");
          if (!tcp.IsLoopClosing()) {
            DoDisconnect("remote end closed connection");
          }
        });
        tcp.closed.connect([this, &tcp] {
          DEBUG3("NT3 TCP connection closed");
          if (!tcp.IsLoopClosing()) {
            DoDisconnect(m_wire ? m_wire->GetDisconnectReason() : "unknown");
          }
        });

        m_clientImpl->SetLocal(&m_localStorage);
        m_localStorage.StartNetwork(&m_localQueue);
        HandleLocal();
      });

  tcp.SetData(clientImpl);
  tcp.data.connect(
      [clientImpl = clientImpl.get()](uv::Buffer& buf, size_t len) {
        clientImpl->ProcessIncoming(
            {reinterpret_cast<const uint8_t*>(buf.base), len});
      });
  tcp.StartRead();
}

void NetworkClient3::ForceDisconnect(std::string_view reason) {
  if (m_wire) {
    m_wire->Disconnect(reason);
  }
}

void NetworkClient3::DoDisconnect(std::string_view reason) {
  INFO("DISCONNECTED NT3 connection: {}", reason);
  m_clientImpl.reset();
  m_wire.reset();
  NetworkClientBase::DoDisconnect(reason);
}

NetworkClient::NetworkClient(
    int inst, std::string_view id, net::ILocalStorage& localStorage,
    IConnectionList& connList, wpi::Logger& logger,
    std::function<void(int64_t serverTimeOffset, int64_t rtt2, bool valid)>
        timeSyncUpdated)
    : NetworkClientBase{inst, id, localStorage, connList, logger},
      m_timeSyncUpdated{std::move(timeSyncUpdated)} {
  m_loopRunner.ExecAsync([this](uv::Loop& loop) {
    m_parallelConnect = wpi::ParallelTcpConnector::Create(
        loop, kReconnectRate, m_logger,
        [this](uv::Tcp& tcp) { TcpConnected(tcp); }, true);

    m_readLocalTimer = uv::Timer::Create(loop);
    if (m_readLocalTimer) {
      m_readLocalTimer->timeout.connect([this] {
        if (m_clientImpl) {
          HandleLocal();
          m_clientImpl->SendOutgoing(m_loop.Now().count(), false);
        }
      });
      m_readLocalTimer->Start(uv::Timer::Time{100}, uv::Timer::Time{100});
    }

    m_sendOutgoingTimer = uv::Timer::Create(loop);
    if (m_sendOutgoingTimer) {
      m_sendOutgoingTimer->timeout.connect([this] {
        if (m_clientImpl) {
          HandleLocal();
          m_clientImpl->SendOutgoing(m_loop.Now().count(), false);
        }
      });
    }

    // set up flush async
    m_flush = uv::Async<>::Create(m_loop);
    if (m_flush) {
      m_flush->wakeup.connect([this] {
        if (m_clientImpl) {
          HandleLocal();
          m_clientImpl->SendOutgoing(m_loop.Now().count(), true);
        }
      });
    }
    m_flushAtomic = m_flush.get();

    m_flushLocal = uv::Async<>::Create(m_loop);
    if (m_flushLocal) {
      m_flushLocal->wakeup.connect([this] { HandleLocal(); });
    }
    m_flushLocalAtomic = m_flushLocal.get();
  });
}

NetworkClient::~NetworkClient() {
  // must explicitly destroy these on loop
  m_loopRunner.ExecSync([&](auto&) {
    m_clientImpl.reset();
    m_wire.reset();
  });
  // shut down loop here to avoid race
  m_loopRunner.Stop();
}

void NetworkClient::HandleLocal() {
  for (;;) {
    auto msgs = m_localQueue.ReadQueue(m_localMsgs);
    if (msgs.empty()) {
      return;
    }
    if (m_clientImpl) {
      m_clientImpl->HandleLocal(msgs);
    }
  }
}

void NetworkClient::TcpConnected(uv::Tcp& tcp) {
  tcp.SetLogger(&m_logger);
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
      {"v4.1.networktables.first.wpi.edu", "networktables.first.wpi.edu"},
      options);
  ws->SetMaxMessageSize(kMaxMessageSize);
  ws->open.connect([this, &tcp, ws = ws.get()](std::string_view protocol) {
    if (m_connList.IsConnected()) {
      ws->Terminate(1006, "no longer needed");
      return;
    }
    WsConnected(*ws, tcp, protocol);
  });
}

void NetworkClient::WsConnected(wpi::WebSocket& ws, uv::Tcp& tcp,
                                std::string_view protocol) {
  if (m_parallelConnect) {
    m_parallelConnect->Succeeded(tcp);
  }

  ConnectionInfo connInfo;
  uv::AddrToName(tcp.GetPeer(), &connInfo.remote_ip, &connInfo.remote_port);
  connInfo.protocol_version =
      protocol == "v4.1.networktables.first.wpi.edu" ? 0x0401 : 0x0400;

  INFO("CONNECTED NT4 to {} port {}", connInfo.remote_ip, connInfo.remote_port);
  m_connHandle = m_connList.AddConnection(connInfo);

  m_wire = std::make_shared<net::WebSocketConnection>(
      ws, connInfo.protocol_version, m_logger);
  m_clientImpl = std::make_unique<net::ClientImpl>(
      m_loop.Now().count(), *m_wire, m_logger, m_timeSyncUpdated,
      [this](uint32_t repeatMs) {
        DEBUG4("Setting periodic timer to {}", repeatMs);
        if (m_sendOutgoingTimer &&
            (!m_sendOutgoingTimer->IsActive() ||
             uv::Timer::Time{repeatMs} != m_sendOutgoingTimer->GetRepeat())) {
          m_sendOutgoingTimer->Start(uv::Timer::Time{repeatMs},
                                     uv::Timer::Time{repeatMs});
        }
      });
  m_clientImpl->SetLocal(&m_localStorage);
  m_localStorage.StartNetwork(&m_localQueue);
  HandleLocal();
  m_clientImpl->SendInitial();
  ws.closed.connect([this, &ws](uint16_t, std::string_view reason) {
    if (!ws.GetStream().IsLoopClosing()) {
      // we could be in the middle of sending data, so defer disconnect
      // capture a shared_ptr copy of ws to make sure it doesn't get destroyed
      // until after DoDisconnect returns
      uv::Timer::SingleShot(
          m_loop, uv::Timer::Time{0},
          [this, reason = std::string{reason}, keepws = ws.shared_from_this()] {
            DoDisconnect(reason);
          });
    }
  });
  ws.text.connect([this](std::string_view data, bool) {
    if (m_clientImpl) {
      m_clientImpl->ProcessIncomingText(data);
    }
  });
  ws.binary.connect([this](std::span<const uint8_t> data, bool) {
    if (m_clientImpl) {
      m_clientImpl->ProcessIncomingBinary(m_loop.Now().count(), data);
    }
  });
}

void NetworkClient::ForceDisconnect(std::string_view reason) {
  if (m_wire) {
    m_wire->Disconnect(reason);
  }
}

void NetworkClient::DoDisconnect(std::string_view reason) {
  std::string realReason;
  if (m_wire) {
    realReason = m_wire->GetDisconnectReason();
  }
  INFO("DISCONNECTED NT4 connection: {}",
       realReason.empty() ? reason : realReason);
  m_clientImpl.reset();
  m_wire.reset();
  NetworkClientBase::DoDisconnect(reason);
  m_timeSyncUpdated(0, 0, false);
}
