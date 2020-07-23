/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALSimWSClientConnection.h"

#include <wpi/raw_ostream.h>
#include <wpi/raw_uv_ostream.h>

#include "HALSimWSClient.h"

namespace uv = wpi::uv;

namespace wpilibws {

void HALSimWSClientConnection::Initialize() {
  // Get a shared pointer to ourselves
  auto self = this->shared_from_this();

  auto hws = HALSimWS::GetInstance();
  std::string reqHost =
      hws->GetTargetHost() + ":" + std::to_string(hws->GetTargetPort());

  auto ws =
      wpi::WebSocket::CreateClient(*m_stream, hws->GetTargetUri(), reqHost);

  ws->SetData(self);

  m_websocket = ws.get();

  // Hook up events
  m_websocket->open.connect_extended([this](auto conn, wpi::StringRef) {
    conn.disconnect();

    m_buffers = std::make_unique<BufferPool>();

    m_exec =
        UvExecFunc::Create(m_stream->GetLoop(), [](auto out, LoopFunc func) {
          func();
          out.set_value();
        });

    auto hws = HALSimWS::GetInstance();
    if (!hws) {
      wpi::errs() << "Unable to get hws instance\n";
      return;
    }

    if (!hws->RegisterWebsocket(GetSharedFromThis())) {
      wpi::errs() << "Unable to register websocket\n";
      return;
    }

    m_ws_connected = true;
    wpi::errs() << "HALSimWS: WebSocket Connected\n";
  });

  m_websocket->text.connect([this](wpi::StringRef msg, bool) {
    auto hws = HALSimWS::GetInstance();
    if (!m_ws_connected || !hws) {
      return;
    }

    wpi::json j;
    try {
      j = wpi::json::parse(msg);
    } catch (const wpi::json::parse_error& e) {
      std::string err("JSON parse failed: ");
      err += e.what();
      wpi::errs() << err << "\n";
      m_websocket->Fail(1003, err);
      return;
    }

    hws->OnNetValueChanged(j);
  });

  m_websocket->closed.connect([this](uint16_t, wpi::StringRef) {
    if (m_ws_connected) {
      wpi::errs() << "HALSimWS: Websocket Disconnected\n";
      m_ws_connected = false;

      auto hws = HALSimWS::GetInstance();
      if (hws) {
        hws->CloseWebsocket(GetSharedFromThis());
      }
    }
  });
}

void HALSimWSClientConnection::OnSimValueChanged(const wpi::json& msg) {
  if (msg.empty()) {
    return;
  }
  wpi::SmallVector<uv::Buffer, 4> sendBufs;
  wpi::raw_uv_ostream os{sendBufs, [this]() -> uv::Buffer {
                           std::lock_guard lock(m_buffers_mutex);
                           return m_buffers->Allocate();
                         }};

  os << msg;

  // Call the websocket send function on the uv loop
  m_exec->Call([this, sendBufs]() mutable {
    m_websocket->SendText(sendBufs, [this](auto bufs, wpi::uv::Error err) {
      {
        std::lock_guard lock(m_buffers_mutex);
        m_buffers->Release(bufs);
      }

      if (err) {
        wpi::errs() << err.str() << "\n";
        wpi::errs().flush();
      }
    });
  });
}

}  // namespace wpilibws
