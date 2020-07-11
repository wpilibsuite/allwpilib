#include "HALSimWebsocketConnection.h"
#include "HALSimWS.h"

#include <wpi/raw_ostream.h>
#include <wpi/raw_uv_ostream.h>

namespace uv = wpi::uv;

void HALSimWebsocketConnection::Initialize() {
  // Get a shared pointer to ourselves
  auto self = this->shared_from_this();
  auto ws = wpi::WebSocket::CreateClient(*m_stream, "/ws", "localhost:8080");

  // Set `this` as the websocket userdata to keep it around
  ws->SetData(self);

  m_websocket = ws.get();

  // Hook into events
  m_websocket->open.connect_extended([this](auto conn, wpi::StringRef) {
    conn.disconnect();

    m_buffers = std::make_unique<BufferPool>();

    m_exec = UvExecFunc::Create(m_stream->GetLoop(), [](auto out, LoopFunc func) {
      func();
      out.set_value();
    });

    auto hws = HALSimWS::GetInstance();
    if (!hws) {
      wpi::errs() << "Unable to get hws instance\n";
      return;
    }

    if (!hws->RegisterWebsocket(shared_from_this())) {
      wpi::errs() << "Unable to register websocket\n";
      return;
    }

    m_ws_connected = true;
    wpi::errs() << "HALSimWS: Websocket Connected\n";
  });

  m_websocket->text.connect([this](wpi::StringRef msg, bool) {
    auto hws = HALSimWS::GetInstance();
    if (!m_ws_connected || !hws) {
      return;
    }

    wpi::json j;
    try {
      j = wpi::json::parse(msg);
    }
    catch (const wpi::json::parse_error& e) {
      std::string err("JSON parse failed: ");
      err += e.what();
      wpi::errs() << err << "\n";
      return;
    }

    hws->OnEndpointValueChanged(j);
  });

  m_websocket->closed.connect([this](uint16_t, wpi::StringRef) {
    if (m_ws_connected) {
      wpi::errs() << "HALSimWS: Websocket disconnected\n";
      m_ws_connected = false;

      auto hws = HALSimWS::GetInstance();
      if (hws) {
        hws->CloseWebsocket(shared_from_this());
      }
    }
  });
}

void HALSimWebsocketConnection::OnSimValueChanged(const wpi::json& msg) {
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