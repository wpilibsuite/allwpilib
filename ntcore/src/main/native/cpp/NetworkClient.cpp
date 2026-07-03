// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "NetworkClient.hpp"

#include <stdint.h>

#include <algorithm>
#include <atomic>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include "IConnectionList.hpp"
#include "Log.hpp"
#include "net/NetworkInterface.hpp"
#include "wpi/net/HttpUtil.hpp"
#include "wpi/net/uv/Loop.hpp"
#include "wpi/net/uv/Tcp.hpp"
#include "wpi/net/uv/util.hpp"
#include "wpi/util/SmallString.hpp"
#include "wpi/util/StringExtras.hpp"

using namespace wpi::nt;
namespace uv = wpi::net::uv;

static constexpr std::string_view kNetworkTablesServiceType =
    "_networktables._tcp";
static constexpr uv::Timer::Time kReconnectRate{1000};
static constexpr uv::Timer::Time kWebsocketHandshakeTimeout{500};
// use a larger max message size for websockets
static constexpr size_t kMaxMessageSize = 2 * 1024 * 1024;

static std::string Ipv4AddressToString(unsigned int address) {
  return fmt::format("{}.{}.{}.{}", (address >> 24) & 0xff,
                     (address >> 16) & 0xff, (address >> 8) & 0xff,
                     address & 0xff);
}

static bool MatchesNetworkTablesResolver(
    const INetworkClient::ServerResolver& resolver,
    const wpi::net::MulticastResolverClient::ServiceData& data) {
  return data.serviceName == resolver.serviceName;
}

NetworkClientBase::NetworkClientBase(int inst, std::string_view id,
                                     net::ILocalStorage& localStorage,
                                     IConnectionList& connList,
                                     wpi::util::Logger& logger)
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
    m_dsClientServer.second = port == 0 ? NT_DEFAULT_PORT : port;
    m_dsClient = wpi::net::DsClient::Create(m_loop, m_logger);
    if (m_dsClient) {
      m_dsClient->setIp.connect([this](std::string_view ip) {
        m_dsClientServer.first = ip;
        if (m_parallelConnect) {
          m_parallelConnect->SetServers({{m_dsClientServer}});
        }
      });
      m_dsClient->clearIp.connect([this] {
        m_dsClientServer.first.clear();
        UpdateConnectorServers();
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
    m_dsClientServer.first.clear();
    UpdateConnectorServers();
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
    std::optional<ServerResolver> resolver, unsigned int defaultPort) {
  std::vector<std::pair<std::string, unsigned int>> serversCopy;
  serversCopy.reserve(servers.size());
  for (auto&& server : servers) {
    serversCopy.emplace_back(wpi::util::trim(server.first),
                             server.second == 0 ? defaultPort : server.second);
  }

  if (resolver) {
    resolver->serviceName = wpi::util::trim(resolver->serviceName);
    if (resolver->team) {
      resolver->team = std::string{wpi::util::trim(*resolver->team)};
    }
    if (resolver->port == 0) {
      resolver->port = defaultPort;
    }
  }

  m_loopRunner.ExecAsync([this, servers = std::move(serversCopy),
                          resolver = std::move(resolver)](uv::Loop&) mutable {
    m_servers = std::move(servers);
    m_serverResolver = std::move(resolver);
    m_resolvedServers.clear();
    StartResolvers();
    UpdateConnectorServers();
  });
}

void NetworkClientBase::DoSetServers(
    std::span<const std::pair<std::string, unsigned int>> servers,
    unsigned int defaultPort) {
  DoSetServers(servers, std::nullopt, defaultPort);
}

void NetworkClientBase::StartResolvers() {
  StopResolvers();

  if (!m_serverResolver || m_connHandle != 0) {
    return;
  }

  if (m_serverResolver->kind == ServerResolver::Kind::kSystemCore) {
    if (m_serverResolver->team) {
      m_systemCoreResolver = wpi::net::SystemCoreResolverClient::Create(
          m_loop, m_logger, *m_serverResolver->team, m_serverResolver->port);
    } else {
      m_systemCoreResolver = wpi::net::SystemCoreResolverClient::Create(
          m_loop, m_logger, m_serverResolver->port);
    }
    if (m_systemCoreResolver) {
      m_systemCoreResolver->serverResolved.connect(
          [this](wpi::net::SystemCoreResolverClient::ServerData data) {
            ProcessSystemCoreData(std::move(data));
          });
    }
  } else {
    m_mdnsResolver = wpi::net::MulticastResolverClient::Create(
        m_loop, m_logger, kNetworkTablesServiceType);
    if (m_mdnsResolver) {
      m_mdnsResolver->serviceResolved.connect(
          [this, resolverConfig = *m_serverResolver](
              wpi::net::MulticastResolverClient::ServiceData data) {
            ProcessResolverData(resolverConfig, std::move(data));
          });
    }
  }
}

void NetworkClientBase::StopResolvers() {
  if (m_mdnsResolver) {
    m_mdnsResolver->Close();
    m_mdnsResolver.reset();
  }
  if (m_systemCoreResolver) {
    m_systemCoreResolver->Close();
    m_systemCoreResolver.reset();
  }
}

void NetworkClientBase::ProcessResolverData(
    const ServerResolver& resolver,
    wpi::net::MulticastResolverClient::ServiceData data) {
  if (!MatchesNetworkTablesResolver(resolver, data)) {
    return;
  }

  unsigned int port = resolver.port;
  if (port == 0) {
    return;
  }

  std::pair<std::string, unsigned int> server{
      Ipv4AddressToString(data.ipv4Address), port};
  if (!AddResolvedServer(server)) {
    return;
  }

  INFO("mDNS resolved service '{}' to {} port {}", data.serviceName,
       server.first, server.second);
  UpdateConnectorServers();
}

void NetworkClientBase::ProcessSystemCoreData(
    wpi::net::SystemCoreResolverClient::ServerData data) {
  std::pair<std::string, unsigned int> server{std::move(data.host), data.port};
  if (!AddResolvedServer(server)) {
    return;
  }

  INFO("SystemCore resolved service '{}' to {} port {}", data.serviceName,
       server.first, server.second);
  UpdateConnectorServers();
}

bool NetworkClientBase::AddResolvedServer(
    std::pair<std::string, unsigned int> server) {
  if (std::find(m_resolvedServers.begin(), m_resolvedServers.end(), server) !=
      m_resolvedServers.end()) {
    return false;
  }

  m_resolvedServers.emplace_back(std::move(server));
  return true;
}

void NetworkClientBase::UpdateConnectorServers() {
  if (!m_parallelConnect || !m_dsClientServer.first.empty()) {
    return;
  }

  std::vector<std::pair<std::string, unsigned int>> servers;
  servers.reserve(m_servers.size() + m_resolvedServers.size());
  servers.insert(servers.end(), m_servers.begin(), m_servers.end());
  servers.insert(servers.end(), m_resolvedServers.begin(),
                 m_resolvedServers.end());
  m_parallelConnect->SetServers(servers);
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
    StartResolvers();
    if (m_parallelConnect) {
      m_parallelConnect->Disconnected();
    }
  });
}

NetworkClient::NetworkClient(
    int inst, std::string_view id, net::ILocalStorage& localStorage,
    IConnectionList& connList, wpi::util::Logger& logger,
    std::function<void(int64_t serverTimeOffset, int64_t rtt2, bool valid)>
        timeSyncUpdated)
    : NetworkClientBase{inst, id, localStorage, connList, logger},
      m_timeSyncUpdated{std::move(timeSyncUpdated)} {
  m_loopRunner.ExecAsync([this](uv::Loop& loop) {
    m_parallelConnect = wpi::net::ParallelTcpConnector::Create(
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
    StopResolvers();
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
  if (m_logger.min_level() >= wpi::util::WPI_LOG_DEBUG4) {
    std::string ip;
    unsigned int port = 0;
    uv::AddrToName(tcp.GetPeer(), &ip, &port);
    DEBUG4("Starting WebSocket client on {} port {}", ip, port);
  }
  wpi::net::WebSocket::ClientOptions options;
  options.handshakeTimeout = kWebsocketHandshakeTimeout;
  wpi::util::SmallString<128> idBuf;
  auto ws = wpi::net::WebSocket::CreateClient(
      tcp, fmt::format("/nt/{}", wpi::net::EscapeURI(m_id, idBuf)), "",
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

void NetworkClient::WsConnected(wpi::net::WebSocket& ws, uv::Tcp& tcp,
                                std::string_view protocol) {
  if (m_parallelConnect) {
    m_parallelConnect->Succeeded(tcp);
  }
  StopResolvers();

  ConnectionInfo connInfo;
  uv::AddrToName(tcp.GetPeer(), &connInfo.remote_ip, &connInfo.remote_port);
  connInfo.protocol_version =
      protocol == "v4.1.networktables.first.wpi.edu" ? 0x0401 : 0x0400;

  INFO("CONNECTED NT4 to {} port {}", connInfo.remote_ip, connInfo.remote_port);
  m_connHandle = m_connList.AddConnection(connInfo);

  bool local = wpi::util::starts_with(connInfo.remote_ip, "127.");

  m_wire = std::make_shared<net::WebSocketConnection>(
      ws, connInfo.protocol_version, m_logger);
  m_clientImpl = std::make_unique<net::ClientImpl>(
      m_loop.Now().count(), *m_wire, local, m_logger, m_timeSyncUpdated,
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
            // Check that the loop is not shutting down. keepws->GetStream()
            // would dereference the underlying uv::Tcp which may have been
            // destroyed after Handle::Close() released its self-reference.
            // m_loop is safe to access as long as `this` is alive.
            if (m_loop.IsClosing()) {
              return;
            }
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
