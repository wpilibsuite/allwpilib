/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/ParallelRepeatTCPConnector.h"

namespace wpi {
namespace uv {

void ParallelRepeatTCPConnector::Start() {
  auto async = Async<>::Create(m_loop);
  m_asyncEnter = async;
  async->wakeup.connect([self = shared_from_this()] {
    int oldConnectCount = 0;
    {
      std::lock_guard<wpi::mutex> lock(self->m_changeAddressesMutex);
      oldConnectCount = self->m_connectingAddresses.size();
      self->m_connectingAddresses.clear();
      self->m_connectingAddresses.append(self->m_changeAddresses.begin(),
                                         self->m_changeAddresses.end());
    }
    if (oldConnectCount == 0) {
      self->Disconnect();
      return;
    }
    if (!self->m_disconnectOnNewAddresses) return;
    self->Disconnect();
  });

  async->closed.connect([self = shared_from_this()] {
    self->closed();
    self->m_this = nullptr;
    self->Disconnect();
  });
}

void ParallelRepeatTCPConnector::Close() {
  if (auto lock = m_asyncEnter.lock()) {
    lock->Close();
  } else {
    // Async is closed. Do the equivelent but make sure to hold this.
    auto localThis = shared_from_this();
    closed();
    m_this = nullptr;
    Disconnect();
  }
}

void ParallelRepeatTCPConnector::Connect() {
  if (!m_this || m_connectingAddresses.empty()) {
    return;
  }

  auto timer = Timer::Create(m_loop);

  m_attemptingConnections.reserve(m_connectingAddresses.size());
  for (auto&& attempt : m_connectingAddresses) {
    // Create a TCP per address
    auto tcp = Tcp::Create(m_loop);
    auto tcpLocal = tcp.get();
    m_attemptingConnections.emplace_back(tcpLocal);
    auto addrInfoReq = std::make_shared<GetAddrInfoReq>();
    std::weak_ptr<GetAddrInfoReq> addrInfoReqWeak = addrInfoReq;

    tcp->closed.connect(
        [ self = shared_from_this(), timer, tcpLocal, addrInfoReqWeak ] {
          auto found = std::find(self->m_attemptingConnections.begin(),
                                 self->m_attemptingConnections.end(), tcpLocal);
          if (found != self->m_attemptingConnections.end()) {
            self->m_attemptingConnections.erase(found);
          }
          if (auto lock = addrInfoReqWeak.lock()) {
            lock->Cancel();
          }
          if (self->m_attemptingConnections.empty()) {
            if (!timer->IsClosing()) {
              timer->Close();
            }
            self->Disconnect();
          }
        });

    addrInfoReq->resolved.connect([ tcp, self = shared_from_this(),
                                    timer ](const addrinfo& info) {
      if (tcp->IsClosing()) return;
      std::array<char, 16> name;
      uv_ip4_name(reinterpret_cast<struct sockaddr_in*>(info.ai_addr),
                  name.data(), 16);
      tcp->Connect(*info.ai_addr, [ self, timer, tcpLocal = tcp.get(), name ] {
        if (!timer->IsClosing()) {
          timer->Close();
        }
        for (auto&& toClose : self->m_attemptingConnections) {
          if (toClose != tcpLocal) {
            toClose->Close();
          }
        }
        self->m_attemptingConnections.clear();
        self->m_attemptingConnections.emplace_back(tcpLocal);
        self->connected(*tcpLocal, StringRef{name.data(), name.size()});
      });
    });

    addrinfo hints;
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = 0;

    GetAddrInfo(m_loop, addrInfoReq, attempt.first, Twine{attempt.second},
                &hints);
  }

  timer->timeout.connect(
      [ self = shared_from_this(), timerLocal = timer.get() ] {
        timerLocal->Close();
        self->Disconnect();
      });

  timer->Start(m_reconnectTime);
}
}  // namespace uv
}  // namespace wpi
