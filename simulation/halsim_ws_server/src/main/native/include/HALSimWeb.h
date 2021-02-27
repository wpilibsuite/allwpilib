// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string>

#include <WSBaseProvider.h>
#include <WSProviderContainer.h>
#include <WSProvider_SimDevice.h>
#include <wpi/StringRef.h>
#include <wpi/uv/Async.h>
#include <wpi/uv/Loop.h>
#include <wpi/uv/Tcp.h>

namespace wpi {
class json;
}  // namespace wpi

namespace wpilibws {

class HALSimWeb : public std::enable_shared_from_this<HALSimWeb> {
 public:
  using LoopFunc = std::function<void(void)>;
  using UvExecFunc = wpi::uv::Async<LoopFunc>;

  HALSimWeb(wpi::uv::Loop& loop, ProviderContainer& providers,
            HALSimWSProviderSimDevices& simDevicesProvider);

  HALSimWeb(const HALSimWeb&) = delete;
  HALSimWeb& operator=(const HALSimWeb&) = delete;

  bool Initialize();
  void Start();

  bool RegisterWebsocket(std::shared_ptr<HALSimBaseWebSocketConnection> hws);
  void CloseWebsocket(std::shared_ptr<HALSimBaseWebSocketConnection> hws);

  // network -> sim
  void OnNetValueChanged(const wpi::json& msg);

  wpi::StringRef GetWebrootSys() const { return m_webroot_sys; }
  wpi::StringRef GetWebrootUser() const { return m_webroot_user; }
  wpi::StringRef GetServerUri() const { return m_uri; }
  int GetServerPort() const { return m_port; }
  wpi::uv::Loop& GetLoop() { return m_loop; }

  UvExecFunc& GetExec() { return *m_exec; }

 private:
  // connected http connection that contains active websocket
  std::weak_ptr<HALSimBaseWebSocketConnection> m_hws;

  wpi::uv::Loop& m_loop;
  std::shared_ptr<wpi::uv::Tcp> m_server;
  std::shared_ptr<UvExecFunc> m_exec;

  // list of providers
  ProviderContainer& m_providers;
  HALSimWSProviderSimDevices& m_simDevicesProvider;

  // Absolute paths of folders to retrieve data from
  // -> /
  std::string m_webroot_sys;
  // -> /user
  std::string m_webroot_user;

  std::string m_uri;
  int m_port;
};

}  // namespace wpilibws
