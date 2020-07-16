#include "HALSimWSClient.h"
#include "HALSimWSClientConnection.h"

#include <wpi/raw_ostream.h>
#include <wpi/uv/util.h>

#define MAX_TCP_CONNECT_ATTEMPTS 5
#define TCP_CONNECT_ATTEMPT_TIMEOUT_MS 1000

namespace uv = wpi::uv;

namespace wpilibws {

std::shared_ptr<HALSimWS> HALSimWS::g_instance;

bool HALSimWS::Initialize() {

  wpi::SmallVector<char, 64> tmp;

  const char* host = getenv("HALSIMWS_HOST");
  if (host != NULL) {
    wpi::StringRef hoststr(host);
    tmp.append(hoststr.begin(), hoststr.end());
    m_host = wpi::Twine(hoststr).str();
  }
  else {
    m_host = "localhost";
  }

  tmp.clear();
  const char* port = getenv("HALSIMWS_PORT");
  if (port != NULL) {
    wpi::StringRef portstr(port);
    tmp.append(portstr.begin(), portstr.end());

    try {
      m_port = std::stoi(wpi::Twine(portstr).str());
    }
    catch (const std::invalid_argument& err) {
      wpi::errs() << "Error decoding HALSIMWS_PORT. Defaulting to 8080. (" << err.what() << ")\n";
      m_port = 8080;
    }
  }
  else {
    m_port = 8080;
  }

  tmp.clear();
  const char* uri = getenv("HALSIMWS_URI");
  if (uri != NULL) {
    wpi::StringRef uristr(uri);
    tmp.append(uristr.begin(), uristr.end());
    m_uri = wpi::Twine(uristr).str();
  }
  else {
    m_uri = "/wpilibws";
  }

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

  // Hook up TCP client events
  m_tcp_client->error.connect([this, socket = m_tcp_client.get()](wpi::uv::Error err) {
    if (m_tcp_connected) {
      m_tcp_connected = false;
      m_connect_attempts = 0;
      m_loop->Stop();
      return;
    }

    // If we weren't previously connected, attempt a reconnection
    m_connect_timer->Start(uv::Timer::Time(TCP_CONNECT_ATTEMPT_TIMEOUT_MS));

  });

  m_tcp_client->closed.connect([this]() {
    wpi::errs() << "TCP connection closed\n";
  });

  // Set up the connection timer
  m_connect_timer = uv::Timer::Create(m_loop);
  if (!m_connect_timer) {
    return false;
  }

  wpi::errs() << "HALSimWS Initialized\n";
  wpi::errs() << "Will attempt to connect to: " << m_host
              << ":" << m_port << " " << m_uri << "\n";

  return true;
}

void HALSimWS::Main(void* param) {
  GetInstance()->MainLoop();
  SetInstance(nullptr);
}

void HALSimWS::MainLoop() {
  // Set up the timer to attempt connection
  m_connect_timer->timeout.connect([this]() {
    AttemptConnect();
  });

  // Run the initial connect immediately
  m_connect_timer->Start(uv::Timer::Time(0));

  m_loop->Run();
}

void HALSimWS::AttemptConnect() {
  m_connect_attempts++;

  if (m_connect_attempts > MAX_TCP_CONNECT_ATTEMPTS) {
    m_loop->Stop();
    return;
  }

  wpi::errs() << "Attempt #" << m_connect_attempts << " of " << MAX_TCP_CONNECT_ATTEMPTS << "\n";

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
  // TODO might need a different format?
  for (auto iter = msg.cbegin(); iter != msg.cend(); ++iter) {
    auto provider = m_providers.Get(iter.key());
    if (provider) {
      provider->OnNetValueChanged(iter.value());
    }
  }
}

}