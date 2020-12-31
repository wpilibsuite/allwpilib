// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string>

#include <WSProviderContainer.h>
#include <WSProvider_SimDevice.h>
#include <wpi/uv/Async.h>
#include <wpi/uv/Loop.h>
#include <wpi/uv/Tcp.h>
#include <wpi/uv/Timer.h>

namespace wpi {
class json;
}  // namespace wpi

namespace wpilibws {

class HALSimWSClientConnection;

class HALSimWS : public std::enable_shared_from_this<HALSimWS> {
 public:
  using LoopFunc = std::function<void(void)>;
  using UvExecFunc = wpi::uv::Async<LoopFunc>;

  HALSimWS(wpi::uv::Loop& loop, ProviderContainer& providers,
           HALSimWSProviderSimDevices& simDevicesProvider);
  HALSimWS(const HALSimWS&) = delete;
  HALSimWS& operator=(const HALSimWS&) = delete;

  bool Initialize();
  void Start();

  bool RegisterWebsocket(std::shared_ptr<HALSimBaseWebSocketConnection> hws);
  void CloseWebsocket(std::shared_ptr<HALSimBaseWebSocketConnection> hws);

  void OnNetValueChanged(const wpi::json& msg);

  wpi::StringRef GetTargetHost() const { return m_host; }
  wpi::StringRef GetTargetUri() const { return m_uri; }
  int GetTargetPort() const { return m_port; }
  wpi::uv::Loop& GetLoop() { return m_loop; }

  UvExecFunc& GetExec() { return *m_exec; }

 private:
  void AttemptConnect();

  bool m_tcp_connected = false;
  std::shared_ptr<wpi::uv::Timer> m_connect_timer;
  int m_connect_attempts = 0;

  std::weak_ptr<HALSimBaseWebSocketConnection> m_hws;

  wpi::uv::Loop& m_loop;
  std::shared_ptr<wpi::uv::Tcp> m_tcp_client;
  std::shared_ptr<UvExecFunc> m_exec;

  ProviderContainer& m_providers;
  HALSimWSProviderSimDevices& m_simDevicesProvider;

  std::string m_host;
  std::string m_uri;
  int m_port;
};

}  // namespace wpilibws
