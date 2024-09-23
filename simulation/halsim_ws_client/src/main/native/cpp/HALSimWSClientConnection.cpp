// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "HALSimWSClientConnection.h"

#include <cstdio>
#include <string>

#include <fmt/format.h>
#include <wpi/print.h>
#include <wpinet/raw_uv_ostream.h>

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
      std::fputs("Unable to register websocket\n", stderr);
      return;
    }

    m_ws_connected = true;
    std::puts("HALSimWS: WebSocket Connected");
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
      wpi::print(stderr, "{}\n", err);
      m_websocket->Fail(1003, err);
      return;
    }

    m_client->OnNetValueChanged(j);
  });

  m_websocket->closed.connect([this](uint16_t, auto) {
    if (m_ws_connected) {
      std::puts("HALSimWS: Websocket Disconnected");
      m_ws_connected = false;

      m_client->CloseWebsocket(shared_from_this());
    }
  });
}

void HALSimWSClientConnection::OnSimValueChanged(const wpi::json& msg) {
  if (msg.empty()) {
    return;
  }

  // Skip sending if this message is not in the allowed filter list
  try {
    auto& type = msg.at("type").get_ref<const std::string&>();
    if (!m_client->CanSendMessage(type)) {
      return;
    }
  } catch (wpi::json::exception& e) {
    wpi::print(stderr, "Error with message: {}\n", e.what());
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
                                    wpi::print(stderr, "{}\n", err.str());
                                    std::fflush(stderr);
                                  }
                                });
  });
}
