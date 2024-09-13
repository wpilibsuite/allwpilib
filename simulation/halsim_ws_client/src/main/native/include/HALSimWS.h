// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>

#include <WSProviderContainer.h>
#include <WSProvider_SimDevice.h>
#include <wpi/StringMap.h>
#include <wpi/json_fwd.h>
#include <wpinet/uv/Async.h>
#include <wpinet/uv/Loop.h>
#include <wpinet/uv/Tcp.h>
#include <wpinet/uv/Timer.h>

namespace wpilibws {

class HALSimWSClientConnection;

class HALSimWS : public std::enable_shared_from_this<HALSimWS> {
 public:
  using LoopFunc = std::function<void()>;
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

  bool CanSendMessage(std::string_view type);

  const std::string& GetTargetHost() const { return m_host; }
  const std::string& GetTargetUri() const { return m_uri; }
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

  bool m_useMsgFiltering;
  wpi::StringMap<bool> m_msgFilters;
};

}  // namespace wpilibws
