/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/RepeatPipeConnector.h"

namespace wpi {
namespace uv {
void RepeatPipeConnector::Start() {
  auto async = Async<>::Create(m_loop);
  m_asyncEnter = async;
  async->wakeup.connect([self = shared_from_this()] {
    {
      std::lock_guard<wpi::mutex> lock(self->m_addressMutex);
      self->m_currentAddress = self->m_changeAddress;
    }
    self->Disconnect();
  });

  async->closed.connect([self = shared_from_this()] {
    self->closed();
    self->m_this = nullptr;
    self->Disconnect();
  });
}

void RepeatPipeConnector::Close() {
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

void RepeatPipeConnector::Connect() {
  if (!m_this || m_currentAddress.empty()) {
    return;
  }

  auto timer = Timer::Create(m_loop);

  auto pipe = Pipe::Create(m_loop, m_ipc);
  auto pipeLocal = pipe.get();
  m_connectingPipe = pipeLocal;

  pipe->closed.connect([ self = shared_from_this(), timer ] {
    self->m_connectingPipe = nullptr;
    if (!timer->IsClosing()) {
      timer->Close();
    }
    self->Disconnect();
  });

  pipe->Connect(m_currentAddress,
                [ pipeLocal, self = shared_from_this(), timer ] {
                  if (!timer->IsClosing()) {
                    timer->Close();
                  }
                  self->connected(*pipeLocal);
                });

  timer->timeout.connect(
      [ self = shared_from_this(), timerLocal = timer.get() ] {
        timerLocal->Close();
        self->Disconnect();
      });

  timer->Start(m_reconnectTime);
}
}  // namespace uv
}  // namespace wpi
