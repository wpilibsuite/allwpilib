/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALSimWSClient.h"

#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>
#include <wpi/uv/util.h>

#include "HALSimWSClientConnection.h"

static constexpr int kTcpConnectAttemptTimeout = 1000;

namespace uv = wpi::uv;

namespace wpilibws {

std::shared_ptr<HALSimWS> HALSimWS::g_instance;

bool HALSimWS::Initialize() {
  const char* host = std::getenv("HALSIMWS_HOST");
  if (host != NULL) {
    m_host = host;
  } else {
    m_host = "localhost";
  }

  const char* port = std::getenv("HALSIMWS_PORT");
  if (port != NULL) {
    try {
      m_port = std::stoi(port);
    } catch (const std::invalid_argument& err) {
      wpi::errs() << "Error decoding HALSIMWS_PORT (" << err.what() << ")\n";
      return false;
    }
  } else {
    m_port = 8080;
  }

  const char* uri = std::getenv("HALSIMWS_URI");
  if (uri != NULL) {
    m_uri = uri;
  } else {
    m_uri = "/wpilibws";
  }

  m_loop = uv::Loop::Create();
  if (!m_loop) {
    return false;
  }

  m_loop->error.connect([](uv::Error err) {
    wpi::errs() << "HALSim WS Client libuv Error: " << err.str() << "\n";
  });

  m_tcp_client = uv::Tcp::Create(m_loop);
  if (!m_tcp_client) {
    return false;
  }

  m_tcp_client->SetNoDelay(true);

  // Hook up TCP client events
  m_tcp_client->error.connect(
      [this, socket = m_tcp_client.get()](wpi::uv::Error err) {
        if (m_tcp_connected) {
          m_tcp_connected = false;
          m_connect_attempts = 0;
          m_loop->Stop();
          return;
        }

        // If we weren't previously connected, attempt a reconnection
        m_connect_timer->Start(uv::Timer::Time(kTcpConnectAttemptTimeout));
      });

  m_tcp_client->closed.connect(
      []() { wpi::outs() << "TCP connection closed\n"; });

  // Set up the connection timer
  m_connect_timer = uv::Timer::Create(m_loop);
  if (!m_connect_timer) {
    return false;
  }

  wpi::outs() << "HALSimWS Initialized\n";
  wpi::outs() << "Will attempt to connect to: " << m_host << ":" << m_port
              << " " << m_uri << "\n";

  return true;
}

void HALSimWS::Main(void* param) {
  GetInstance()->MainLoop();
  SetInstance(nullptr);
}

void HALSimWS::MainLoop() {
  // Set up the timer to attempt connection
  m_connect_timer->timeout.connect([this] { AttemptConnect(); });

  // Run the initial connect immediately
  m_connect_timer->Start(uv::Timer::Time(0));

  m_loop->Run();
}

void HALSimWS::AttemptConnect() {
  m_connect_attempts++;

  wpi::outs() << "Connection Attempt " << m_connect_attempts << "\n";

  struct sockaddr_in dest;
  uv::NameToAddr(m_host, m_port, &dest);

  m_tcp_client->Connect(dest, [this, socket = m_tcp_client.get()]() {
    m_tcp_connected = true;
    auto wsConn = std::make_shared<HALSimWSClientConnection>(m_tcp_client);

    wsConn->Initialize();
  });
}

void HALSimWS::Exit(void* param) {
  auto inst = GetInstance();
  if (!inst) {
    return;
  }

  auto loop = inst->m_loop;
  loop->Walk([](uv::Handle& h) {
    h.SetLoopClosing(true);
    h.Close();
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
    key.append("/");
    key.append(device);

    auto provider = m_providers.Get(key.str());
    if (provider) {
      provider->OnNetValueChanged(msg.at("data"));
    }
  } catch (wpi::json::exception& e) {
    wpi::errs() << "Error with incoming message: " << e.what() << "\n";
  }
}

}  // namespace wpilibws
