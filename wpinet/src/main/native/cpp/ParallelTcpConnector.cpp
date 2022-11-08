// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/ParallelTcpConnector.h"

#include <fmt/format.h>
#include <wpi/Logger.h>

#include "wpinet/uv/GetAddrInfo.h"
#include "wpinet/uv/Loop.h"
#include "wpinet/uv/Tcp.h"
#include "wpinet/uv/Timer.h"
#include "wpinet/uv/util.h"

using namespace wpi;

ParallelTcpConnector::ParallelTcpConnector(
    wpi::uv::Loop& loop, wpi::uv::Timer::Time reconnectRate,
    wpi::Logger& logger, std::function<void(wpi::uv::Tcp& tcp)> connected,
    const private_init&)
    : m_loop{loop},
      m_logger{logger},
      m_reconnectRate{reconnectRate},
      m_connected{std::move(connected)},
      m_reconnectTimer{uv::Timer::Create(loop)} {
  m_reconnectTimer->timeout.connect([this] {
    if (!IsConnected()) {
      WPI_DEBUG1(m_logger, "timed out, reconnecting");
      Connect();
    }
  });
}

ParallelTcpConnector::~ParallelTcpConnector() = default;

void ParallelTcpConnector::Close() {
  CancelAll();
  m_reconnectTimer->Close();
}

void ParallelTcpConnector::SetServers(
    std::span<const std::pair<std::string, unsigned int>> servers) {
  m_servers.assign(servers.begin(), servers.end());
  if (!IsConnected()) {
    Connect();
  }
}

void ParallelTcpConnector::Disconnected() {
  if (m_isConnected) {
    m_isConnected = false;
    Connect();
  }
}

void ParallelTcpConnector::Succeeded(uv::Tcp& tcp) {
  if (!m_isConnected) {
    m_isConnected = true;
    m_reconnectTimer->Stop();
    CancelAll(&tcp);
  }
}

void ParallelTcpConnector::Connect() {
  if (IsConnected()) {
    return;
  }

  CancelAll();
  m_reconnectTimer->Start(m_reconnectRate);

  WPI_DEBUG3(m_logger, "starting new connection attempts");

  // kick off parallel lookups
  for (auto&& server : m_servers) {
    auto req = std::make_shared<uv::GetAddrInfoReq>();
    m_resolvers.emplace_back(req);

    req->resolved.connect(
        [this, req = req.get()](const addrinfo& addrinfo) {
          if (IsConnected()) {
            return;
          }

          // kick off parallel connection attempts
          for (auto ai = &addrinfo; ai; ai = ai->ai_next) {
            auto tcp = uv::Tcp::Create(m_loop);
            m_attempts.emplace_back(tcp);

            auto connreq = std::make_shared<uv::TcpConnectReq>();
            connreq->connected.connect(
                [this, tcp = tcp.get()] {
                  if (m_logger.min_level() <= wpi::WPI_LOG_DEBUG4) {
                    std::string ip;
                    unsigned int port = 0;
                    uv::AddrToName(tcp->GetPeer(), &ip, &port);
                    WPI_DEBUG4(m_logger,
                               "successful connection ({}) to {} port {}",
                               static_cast<void*>(tcp), ip, port);
                  }
                  if (IsConnected()) {
                    tcp->Shutdown([tcp] { tcp->Close(); });
                    return;
                  }
                  if (m_connected) {
                    m_connected(*tcp);
                  }
                },
                shared_from_this());

            connreq->error = [selfWeak = weak_from_this(),
                              tcp = tcp.get()](uv::Error err) {
              if (auto self = selfWeak.lock()) {
                WPI_DEBUG1(self->m_logger, "connect failure ({}): {}",
                           static_cast<void*>(tcp), err.str());
              }
            };

            if (m_logger.min_level() <= wpi::WPI_LOG_DEBUG4) {
              std::string ip;
              unsigned int port = 0;
              uv::AddrToName(*reinterpret_cast<sockaddr_storage*>(ai->ai_addr),
                             &ip, &port);
              WPI_DEBUG4(
                  m_logger,
                  "Info({}) starting connection attempt ({}) to {} port {}",
                  static_cast<void*>(req), static_cast<void*>(tcp.get()), ip,
                  port);
            }
            tcp->Connect(*ai->ai_addr, connreq);
          }
        },
        shared_from_this());

    req->error = [req = req.get(), selfWeak = weak_from_this()](uv::Error err) {
      if (auto self = selfWeak.lock()) {
        WPI_DEBUG1(self->m_logger, "GetAddrInfo({}) failure: {}",
                   static_cast<void*>(req), err.str());
      }
    };

    WPI_DEBUG4(m_logger, "starting GetAddrInfo({}) for {} port {}",
               static_cast<void*>(req.get()), server.first, server.second);
    addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_NUMERICSERV | AI_ADDRCONFIG;
    uv::GetAddrInfo(m_loop, req, server.first, fmt::format("{}", server.second),
                    &hints);
  }
}

void ParallelTcpConnector::CancelAll(wpi::uv::Tcp* except) {
  WPI_DEBUG4(m_logger, "canceling previous attempts");
  for (auto&& resolverWeak : m_resolvers) {
    if (auto resolver = resolverWeak.lock()) {
      WPI_DEBUG4(m_logger, "canceling GetAddrInfo({})",
                 static_cast<void*>(resolver.get()));
      resolver->Cancel();
    }
  }
  m_resolvers.clear();

  for (auto&& tcpWeak : m_attempts) {
    if (auto tcp = tcpWeak.lock()) {
      if (tcp.get() != except) {
        WPI_DEBUG4(m_logger, "canceling connection attempt ({})",
                   static_cast<void*>(tcp.get()));
        tcp->Close();
      }
    }
  }
  m_attempts.clear();
}
