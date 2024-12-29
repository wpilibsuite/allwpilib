// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WebServerClientTest.h"

#include <cstdio>
#include <memory>
#include <string>

#include <fmt/format.h>
#include <wpi/SmallString.h>
#include <wpi/print.h>
#include <wpinet/raw_uv_ostream.h>
#include <wpinet/uv/util.h>

static constexpr int kTcpConnectAttemptTimeout = 1000;

namespace uv = wpi::uv;

namespace wpilibws {

// Create Web Socket and specify event callbacks
void WebServerClientTest::InitializeWebSocket(const std::string& host, int port,
                                              const std::string& uri) {
  wpi::print("Will attempt to connect to: {}:{}{}\n", host, port, uri);
  m_websocket = wpi::WebSocket::CreateClient(*m_tcp_client.get(), uri,
                                             fmt::format("{}:{}", host, port));

  // Hook up events
  m_websocket->open.connect_extended([this](auto conn, auto) {
    conn.disconnect();
    m_buffers = std::make_unique<BufferPool>();

    m_exec = UvExecFunc::Create(m_tcp_client->GetLoop(),
                                [](auto out, LoopFunc func) {
                                  func();
                                  out.set_value();
                                });

    m_ws_connected = true;
    std::fputs("WebServerClientTest: WebSocket Connected\n", stderr);
  });

  m_websocket->text.connect([this](auto msg, bool) {
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
    // Save last message received
    m_json = j;
  });

  m_websocket->closed.connect([this](uint16_t, auto) {
    if (m_ws_connected) {
      std::fputs("WebServerClientTest: Websocket Disconnected\n", stderr);
      m_ws_connected = false;
    }
  });
}

// Create tcp client, specify callbacks, and create timers for loop
bool WebServerClientTest::Initialize() {
  m_loop.error.connect(
      [](uv::Error err) { wpi::print(stderr, "uv Error: {}\n", err.str()); });

  m_tcp_client = uv::Tcp::Create(m_loop);
  if (!m_tcp_client) {
    std::fputs("ERROR: Could not create TCP Client\n", stderr);
    return false;
  }

  // Hook up TCP client events
  m_tcp_client->error.connect(
      [this, socket = m_tcp_client.get()](wpi::uv::Error err) {
        if (m_tcp_connected) {
          m_tcp_connected = false;
          m_connect_attempts = 0;
          m_loop.Stop();
          return;
        }

        // If we weren't previously connected, attempt a reconnection
        m_connect_timer->Start(uv::Timer::Time(kTcpConnectAttemptTimeout));
      });

  m_tcp_client->closed.connect(
      []() { std::fputs("TCP connection closed\n", stderr); });

  // Set up the connection timer
  m_connect_timer = uv::Timer::Create(m_loop);
  if (!m_connect_timer) {
    return false;
  }
  // Set up the timer to attempt connection
  m_connect_timer->timeout.connect([this] { AttemptConnect(); });
  m_connect_timer->Start(uv::Timer::Time(0));

  std::puts("WebServerClientTest Initialized");

  return true;
}

void WebServerClientTest::AttemptConnect() {
  m_connect_attempts++;
  wpi::print("Test Client Connection Attempt {}\n", m_connect_attempts);

  if (m_connect_attempts >= 5) {
    std::fputs("Test Client Timeout. Unable to connect\n", stderr);
    m_loop.Stop();
    return;
  }

  struct sockaddr_in dest;
  uv::NameToAddr("localhost", 3300, &dest);
  m_tcp_client->Connect(dest, [this]() {
    m_tcp_connected = true;
    InitializeWebSocket("localhost", 3300, "/wpilibws");
  });
}

void WebServerClientTest::SendMessage(const wpi::json& msg) {
  if (msg.empty()) {
    std::fputs("Message to send is empty\n", stderr);
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
        wpi::print(stderr, "{}\n", err.str());
        std::fflush(stderr);
      }
    });
  });
}

const wpi::json& WebServerClientTest::GetLastMessage() {
  return m_json;
}

}  // namespace wpilibws
