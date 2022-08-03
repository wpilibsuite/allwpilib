// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "HALSimWS.h"

#include <cstdio>

#include <fmt/format.h>
#include <wpi/SmallString.h>
#include <wpinet/uv/util.h>

#include "HALSimWSClientConnection.h"

static constexpr int kTcpConnectAttemptTimeout = 1000;

namespace uv = wpi::uv;

using namespace wpilibws;

HALSimWS::HALSimWS(wpi::uv::Loop& loop, ProviderContainer& providers,
                   HALSimWSProviderSimDevices& simDevicesProvider)
    : m_loop(loop),
      m_providers(providers),
      m_simDevicesProvider(simDevicesProvider) {
  m_loop.error.connect([](uv::Error err) {
    fmt::print(stderr, "HALSim WS Client libuv Error: {}\n", err.str());
  });

  m_tcp_client = uv::Tcp::Create(m_loop);
  m_exec = UvExecFunc::Create(m_loop);
  if (m_exec) {
    m_exec->wakeup.connect([](auto func) { func(); });
  }
  m_connect_timer = uv::Timer::Create(m_loop);
}

bool HALSimWS::Initialize() {
  if (!m_tcp_client || !m_exec || !m_connect_timer) {
    return false;
  }

  const char* host = std::getenv("HALSIMWS_HOST");
  if (host != nullptr) {
    m_host = host;
  } else {
    m_host = "localhost";
  }

  const char* port = std::getenv("HALSIMWS_PORT");
  if (port != nullptr) {
    try {
      m_port = std::stoi(port);
    } catch (const std::invalid_argument& err) {
      fmt::print(stderr, "Error decoding HALSIMWS_PORT ({})\n", err.what());
      return false;
    }
  } else {
    m_port = 3300;
  }

  const char* uri = std::getenv("HALSIMWS_URI");
  if (uri != nullptr) {
    m_uri = uri;
  } else {
    m_uri = "/wpilibws";
  }

  return true;
}

void HALSimWS::Start() {
  m_tcp_client->SetNoDelay(true);

  // Hook up TCP client events
  m_tcp_client->error.connect(
      [this, socket = m_tcp_client.get()](wpi::uv::Error err) {
        if (m_tcp_connected) {
          m_tcp_connected = false;
          m_connect_attempts = 0;
          return;
        }

        // If we weren't previously connected, attempt a reconnection
        m_connect_timer->Start(uv::Timer::Time(kTcpConnectAttemptTimeout));
      });

  m_tcp_client->closed.connect([]() { std::puts("TCP connection closed"); });

  // Set up the connection timer
  std::puts("HALSimWS Initialized");
  fmt::print("Will attempt to connect to ws://{}:{}{}\n", m_host, m_port,
             m_uri);

  // Set up the timer to attempt connection
  m_connect_timer->timeout.connect([this] { AttemptConnect(); });

  // Run the initial connect immediately
  m_connect_timer->Start(uv::Timer::Time(0));
  m_connect_timer->Unreference();
}

void HALSimWS::AttemptConnect() {
  m_connect_attempts++;

  fmt::print("Connection Attempt {}\n", m_connect_attempts);

  struct sockaddr_in dest;
  uv::NameToAddr(m_host, m_port, &dest);

  m_tcp_client->Connect(dest, [this, socket = m_tcp_client.get()]() {
    m_tcp_connected = true;
    auto wsConn = std::make_shared<HALSimWSClientConnection>(shared_from_this(),
                                                             m_tcp_client);

    wsConn->Initialize();
  });
}

bool HALSimWS::RegisterWebsocket(
    std::shared_ptr<HALSimBaseWebSocketConnection> hws) {
  if (m_hws.lock()) {
    return false;
  }

  m_hws = hws;

  m_simDevicesProvider.OnNetworkConnected(hws);

  m_providers.ForEach([hws](std::shared_ptr<HALSimWSBaseProvider> provider) {
    provider->OnNetworkConnected(hws);
  });

  return true;
}

void HALSimWS::CloseWebsocket(
    std::shared_ptr<HALSimBaseWebSocketConnection> hws) {
  // Inform the providers that they need to cancel callbacks
  m_simDevicesProvider.OnNetworkDisconnected();

  m_providers.ForEach([](std::shared_ptr<HALSimWSBaseProvider> provider) {
    provider->OnNetworkDisconnected();
  });

  if (hws == m_hws.lock()) {
    m_hws.reset();
  }
}

void HALSimWS::OnNetValueChanged(const wpi::json& msg) {
  // Look for "type" and "device" fields so that we can
  // generate the key

  try {
    auto& type = msg.at("type").get_ref<const std::string&>();
    auto& device = msg.at("device").get_ref<const std::string&>();

    wpi::SmallString<64> key;
    key.append(type);
    if (!device.empty()) {
      key.append("/");
      key.append(device);
    }

    auto provider = m_providers.Get(key.str());
    if (provider) {
      provider->OnNetValueChanged(msg.at("data"));
    }
  } catch (wpi::json::exception& e) {
    fmt::print(stderr, "Error with incoming message: {}\n", e.what());
  }
}
