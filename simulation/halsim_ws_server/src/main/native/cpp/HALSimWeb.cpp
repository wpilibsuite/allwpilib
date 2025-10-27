// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/ws_server/HALSimWeb.hpp"

#include <memory>
#include <string>

#include <wpi/net/UrlParser.hpp>
#include <wpi/net/WebSocketServer.hpp>
#include <wpi/net/raw_uv_ostream.hpp>
#include <wpi/net/uv/Loop.hpp>
#include <wpi/net/uv/Tcp.hpp>
#include <wpi/util/SmallString.hpp>
#include <wpi/util/fs.hpp>
#include <wpi/util/print.hpp>

#include "wpi/halsim/ws_server/HALSimHttpConnection.hpp"

namespace uv = wpi::net::uv;

using namespace wpilibws;

HALSimWeb::HALSimWeb(wpi::net::uv::Loop& loop, ProviderContainer& providers,
                     HALSimWSProviderSimDevices& simDevicesProvider)
    : m_loop(loop),
      m_providers(providers),
      m_simDevicesProvider(simDevicesProvider) {
  m_loop.error.connect([](uv::Error err) {
    wpi::util::print(stderr, "HALSim WS Server libuv ERROR: {}\n", err.str());
  });

  m_server = uv::Tcp::Create(m_loop);
  m_exec = UvExecFunc::Create(m_loop);
  if (m_exec) {
    m_exec->wakeup.connect([](auto func) { func(); });
  }
}

bool HALSimWeb::Initialize() {
  if (!m_server || !m_exec) {
    return false;
  }

  // determine where to get static content from
  fs::path path;
  const char* webroot_sys = std::getenv("HALSIMWS_SYSROOT");
  if (webroot_sys != nullptr) {
    path = webroot_sys;
  } else {
    path = fs::current_path() / "sim";
  }
  m_webroot_sys = fs::absolute(path).string();

  const char* webroot_user = std::getenv("HALSIMWS_USERROOT");
  if (webroot_user != nullptr) {
    path = webroot_sys;
  } else {
    path = fs::current_path() / "sim" / "user";
  }
  m_webroot_user = fs::absolute(path).string();

  const char* uri = std::getenv("HALSIMWS_URI");
  if (uri != nullptr) {
    m_uri = uri;
  } else {
    m_uri = "/wpilibws";
  }

  const char* port = std::getenv("HALSIMWS_PORT");
  if (port != nullptr) {
    try {
      m_port = std::stoi(port);
    } catch (const std::invalid_argument& err) {
      wpi::util::print(stderr, "Error decoding HALSIMWS_PORT ({})\n", err.what());
      return false;
    }
  } else {
    m_port = 3300;
  }

  const char* msgFilters = std::getenv("HALSIMWS_FILTERS");
  if (msgFilters != nullptr) {
    m_useMsgFiltering = true;
    wpi::util::split(wpi::util::trim(msgFilters), ',', -1, false,
               [&](auto val) { m_msgFilters[wpi::util::trim(val)] = true; });
  } else {
    m_useMsgFiltering = false;
  }

  return true;
}

void HALSimWeb::Start() {
  m_server->Bind("", m_port);

  // when we get a connection, accept it and start reading
  m_server->connection.connect([this, srv = m_server.get()] {
    auto tcp = srv->Accept();
    if (!tcp) {
      return;
    }

    tcp->SetNoDelay(true);

    auto conn = std::make_shared<HALSimHttpConnection>(shared_from_this(), tcp);
    tcp->SetData(conn);
  });

  // start listening for incoming connections
  m_server->Listen();
  wpi::util::print("Listening at http://localhost:{}\n", m_port);
  wpi::util::print("WebSocket URI: {}\n", m_uri);

  // Print any filters we are using
  if (m_useMsgFiltering) {
    wpi::util::print("WS Message Filters:");
    for (auto&& filter : m_msgFilters) {
      wpi::util::print("* \"{}\"\n", filter.first);
    }
  } else {
    wpi::util::print("No WS Message Filters specified");
  }
}

bool HALSimWeb::RegisterWebsocket(
    std::shared_ptr<HALSimBaseWebSocketConnection> hws) {
  if (m_hws.lock()) {
    return false;
  }

  m_hws = hws;

  m_simDevicesProvider.OnNetworkConnected(hws);

  // notify all providers that they should use this new websocket instead
  m_providers.ForEach([hws](std::shared_ptr<HALSimWSBaseProvider> provider) {
    provider->OnNetworkConnected(hws);
  });

  return true;
}

void HALSimWeb::CloseWebsocket(
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

void HALSimWeb::OnNetValueChanged(const wpi::util::json& msg) {
  // Look for "type" and "device" fields so that we can
  // generate the key

  try {
    auto& type = msg.at("type").get_ref<const std::string&>();
    auto& device = msg.at("device").get_ref<const std::string&>();

    wpi::util::SmallString<64> key;
    key.append(type);
    if (!device.empty()) {
      key.append("/");
      key.append(device);
    }

    auto provider = m_providers.Get(key.str());
    if (provider) {
      provider->OnNetValueChanged(msg.at("data"));
    }
  } catch (wpi::util::json::exception& e) {
    wpi::util::print(stderr, "Error with incoming message: {}\n", e.what());
  }
}

bool HALSimWeb::CanSendMessage(std::string_view type) {
  if (!m_useMsgFiltering) {
    return true;
  }
  return m_msgFilters.count(type) > 0;
}
