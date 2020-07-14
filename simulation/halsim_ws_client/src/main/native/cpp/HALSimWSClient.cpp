#include "HALSimWSClient.h"
#include "HALSimWSClientConnection.h"

#include <wpi/raw_ostream.h>
#include <wpi/uv/util.h>

namespace uv = wpi::uv;

namespace wpilibws {

std::shared_ptr<HALSimWS> HALSimWS::g_instance;

bool HALSimWS::Initialize() {
  m_loop = uv::Loop::Create();
  if (!m_loop) {
    return false;
  }

  m_loop->error.connect([](uv::Error err) {
    wpi::errs() << "uv Error: " << err.str() << "\n";
  });

  m_tcp_client = uv::Tcp::Create(m_loop);
  if (!m_tcp_client) {
    return false;
  }

  wpi::errs() << "HALSimWS Initialized\n";
  return true;
}

void HALSimWS::Main(void* param) {
  GetInstance()->MainLoop();
  SetInstance(nullptr);
}

void HALSimWS::MainLoop() {
  // TODO Implement with environment vars
  struct sockaddr_in dest;
  uv::NameToAddr("localhost", 8080, &dest);

  m_tcp_client->Connect(dest, [this, socket = m_tcp_client.get()]() {
    auto wsConn = std::make_shared<HALSimWSClientConnection>(m_tcp_client);

    wsConn->Initialize();
  });

  m_loop->Run();
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

bool HALSimWS::RegisterWebsocket(std::shared_ptr<HALSimBaseWebSocketConnection> hws) {
  if (m_hws.lock()) {
    return false;
  }

  m_hws = hws;

  m_providers.ForEach([hws](std::shared_ptr<HALSimWSBaseProvider> provider) {
    provider->OnNetworkConnected(hws);
  });

  return true;
}

void HALSimWS::CloseWebsocket(std::shared_ptr<HALSimBaseWebSocketConnection> hws) {
  if (hws == m_hws.lock()) {
    m_hws.reset();
  }
}

void HALSimWS::OnNetValueChanged(const wpi::json& msg) {
  // TODO Implement
}

}