// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "HALSimWSClientConnection.h"

#include <fmt/format.h>
#include <wpi/raw_ostream.h>
#include <wpi/raw_uv_ostream.h>

#include "HALSimWS.h"

namespace uv = wpi::uv;

using namespace wpilibws;

void HALSimWSClientConnection::Initialize() {
  // Get a shared pointer to ourselves
  auto self = this->shared_from_this();

  auto ws = wpi::WebSocket::CreateClient(
      *m_stream, m_client->GetTargetUri(),
      fmt::format("{}:{}", m_client->GetTargetHost(),
                  m_client->GetTargetPort()));

  ws->SetData(self);

  m_websocket = ws.get();

  // Hook up events
  m_websocket->open.connect_extended([this](auto conn, auto) {
    conn.disconnect();

    if (!m_client->RegisterWebsocket(shared_from_this())) {
      wpi::errs() << "Unable to register websocket\n";
      return;
    }

    m_ws_connected = true;
    wpi::outs() << "HALSimWS: WebSocket Connected\n";
  });

  m_websocket->text.connect([this](auto msg, bool) {
    if (!m_ws_connected) {
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

    m_client->OnNetValueChanged(j);
  });

  m_websocket->closed.connect([this](uint16_t, auto) {
    if (m_ws_connected) {
      wpi::outs() << "HALSimWS: Websocket Disconnected\n";
      m_ws_connected = false;

      m_client->CloseWebsocket(shared_from_this());
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
                           return m_buffers.Allocate();
                         }};

  os << msg;

  // Call the websocket send function on the uv loop
  m_client->GetExec().Send([self = shared_from_this(), sendBufs] {
    self->m_websocket->SendText(sendBufs,
                                [self](auto bufs, wpi::uv::Error err) {
                                  {
                                    std::lock_guard lock(self->m_buffers_mutex);
                                    self->m_buffers.Release(bufs);
                                  }

                                  if (err) {
                                    wpi::errs() << err.str() << "\n";
                                    wpi::errs().flush();
                                  }
                                });
  });
}
