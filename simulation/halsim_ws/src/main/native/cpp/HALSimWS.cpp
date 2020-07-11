#include "HALSimWS.h"
#include "HALSimWebsocketConnection.h"
#include "MessageSchema.h"

#include <wpi/raw_ostream.h>
#include <wpi/uv/util.h>

namespace uv = wpi::uv;

std::shared_ptr<HALSimWS> HALSimWS::g_instance;

bool HALSimWS::Initialize() {
  // Create the libuv loop
  m_loop = uv::Loop::Create();
  if (!m_loop) {
    return false;
  }

  m_loop->error.connect([](uv::Error err) {
    wpi::errs() << "uv ERROR: " << err.str() << "\n";
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
  wpi::errs() << "Running MainLoop\n";
  struct sockaddr_in dest;
  uv::NameToAddr("localhost", 8080, &dest);

  m_tcp_client->Connect(dest, [this, socket = m_tcp_client.get()]() {
    wpi::errs() << "About to create WebsocketConnection\n";
    // This section is potentially problematic
    auto wsConn = std::make_shared<HALSimWebsocketConnection>(m_tcp_client);

    wpi::errs() << "About to initialize WebSocket connection\n";
    wsConn->Initialize();


    // socket->SetData(wsConn);
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

bool HALSimWS::RegisterWebsocket(std::shared_ptr<HALSimWebsocketConnection> hws) {
  if (m_hws.lock()) {
    return false;
  }

  m_hws = hws;

  m_providers.ForEach([hws](std::shared_ptr<BaseProvider> provider) {
    provider->OnNetworkConnected(hws);
  });
  // TODO Notify providers to use this socket

  return true;
}

void HALSimWS::CloseWebsocket(std::shared_ptr<HALSimWebsocketConnection> hws) {
  if (hws == m_hws.lock()) {
    m_hws.reset();
  }
}

void HALSimWS::OnEndpointValueChanged(const wpi::json& msg) {
  // Bail out if this is not a valid message structure
  // we should have `topic` and `payload` fields
  if (msg.find("topic") == msg.end() ||
      msg.find("payload") == msg.end()) {
    return;
  }

  auto topic = msg.at("topic").get<std::string>();

  // Decide what to do with the topics here
  if (topic == "digital/in") {
    // TODO Verify payload validity?
    auto payload = msg.at("payload").get<wpilibws::digitalio::value::Message>();

    // Generate the provider key for targeting
    auto providerKey = ("DIO/" + wpi::Twine(payload.channel)).str();
    auto provider = m_providers.Get(providerKey);
    if (provider) {
      provider->OnEndpointValueChanged(msg);
    }
  }
  else if (topic == "analog/in") {
    auto payload = msg.at("payload").get<wpilibws::analogio::value::Message>();

    auto providerKey = ("AIN/" + wpi::Twine(payload.channel)).str();
    auto provider = m_providers.Get(providerKey);
    if (provider) {
      provider->OnEndpointValueChanged(msg);
    }
  }

}