/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALSimWeb.h"

#include <wpi/FileSystem.h>
#include <wpi/Path.h>
#include <wpi/SmallString.h>
#include <wpi/Twine.h>
#include <wpi/UrlParser.h>
#include <wpi/WebSocketServer.h>
#include <wpi/raw_uv_ostream.h>
#include <wpi/uv/Loop.h>
#include <wpi/uv/Tcp.h>

#include "HALSimHttpConnection.h"

namespace uv = wpi::uv;

using namespace wpilibws;

HALSimWeb::HALSimWeb(wpi::uv::Loop& loop, ProviderContainer& providers,
                     HALSimWSProviderSimDevices& simDevicesProvider)
    : m_loop(loop),
      m_providers(providers),
      m_simDevicesProvider(simDevicesProvider) {
  m_loop.error.connect([](uv::Error err) {
    wpi::errs() << "HALSim WS Server libuv ERROR: " << err.str() << '\n';
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
  // wpi::SmallVector<char, 64> tmp;
  wpi::SmallString<64> tmp;

  const char* webroot_sys = std::getenv("HALSIMWS_SYSROOT");
  if (webroot_sys != NULL) {
    wpi::StringRef tstr(webroot_sys);
    tmp.append(tstr);
  } else {
    wpi::sys::fs::current_path(tmp);
    wpi::sys::path::append(tmp, "sim");
  }
  wpi::sys::fs::make_absolute(tmp);
  m_webroot_sys = wpi::Twine(tmp).str();

  tmp.clear();
  const char* webroot_user = std::getenv("HALSIMWS_USERROOT");
  if (webroot_user != NULL) {
    wpi::StringRef tstr(webroot_user);
    tmp.append(tstr);
  } else {
    wpi::sys::fs::current_path(tmp);
    wpi::sys::path::append(tmp, "sim", "user");
  }
  wpi::sys::fs::make_absolute(tmp);
  m_webroot_user = wpi::Twine(tmp).str();

  const char* uri = std::getenv("HALSIMWS_URI");
  if (uri != NULL) {
    m_uri = uri;
  } else {
    m_uri = "/wpilibws";
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
    m_port = 3300;
  }

  return true;
}

void HALSimWeb::Start() {
  m_server->Bind("", m_port);

  // when we get a connection, accept it and start reading
  m_server->connection.connect([this, srv = m_server.get()] {
    auto tcp = srv->Accept();
    if (!tcp) return;

    tcp->SetNoDelay(true);

    auto conn = std::make_shared<HALSimHttpConnection>(shared_from_this(), tcp);
    tcp->SetData(conn);
  });

  // start listening for incoming connections
  m_server->Listen();
  wpi::outs() << "Listening at http://localhost:" << m_port << "\n";
  wpi::outs() << "WebSocket URI: " << m_uri << "\n";
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

void HALSimWeb::OnNetValueChanged(const wpi::json& msg) {
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
    wpi::errs() << "Error with incoming message: " << e.what() << "\n";
  }
}
