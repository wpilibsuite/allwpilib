// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <span>
#include <string>

#include <HALSimBaseWebSocketConnection.h>
#include <WSProviderContainer.h>
#include <WSProvider_SimDevice.h>
#include <wpi/json_fwd.h>
#include <wpinet/uv/Async.h>
#include <wpinet/uv/Buffer.h>
#include <wpinet/uv/Loop.h>
#include <wpinet/uv/Timer.h>
#include <wpinet/uv/Udp.h>

#include "XRP.h"

namespace wpilibxrp {

// This masquerades as a "WebSocket" so that we can reuse the
// stuff in halsim_ws_core
class HALSimXRP : public wpilibws::HALSimBaseWebSocketConnection,
                  public std::enable_shared_from_this<HALSimXRP> {
 public:
  using LoopFunc = std::function<void()>;
  using UvExecFunc = wpi::uv::Async<LoopFunc>;

  HALSimXRP(wpi::uv::Loop& loop, wpilibws::ProviderContainer& providers,
            wpilibws::HALSimWSProviderSimDevices& simDevicesProvider);
  HALSimXRP(const HALSimXRP&) = delete;
  HALSimXRP& operator=(const HALSimXRP&) = delete;

  bool Initialize();
  void Start();

  void ParsePacket(std::span<const uint8_t> packet);
  void OnNetValueChanged(const wpi::json& msg);
  void OnSimValueChanged(const wpi::json& simData) override;

  const std::string& GetTargetHost() const { return m_host; }
  int GetTargetPort() const { return m_port; }
  wpi::uv::Loop& GetLoop() { return m_loop; }

  UvExecFunc& GetExec() { return *m_exec; }

 private:
  XRP m_xrp;

  wpi::uv::Loop& m_loop;
  std::shared_ptr<wpi::uv::Udp> m_udp_client;
  std::shared_ptr<UvExecFunc> m_exec;

  wpilibws::ProviderContainer& m_providers;
  wpilibws::HALSimWSProviderSimDevices& m_simDevicesProvider;

  std::string m_host;
  int m_port;

  void SendStateToXRP();
  wpi::uv::SimpleBufferPool<4>& GetBufferPool();
  std::mutex m_buffer_mutex;

  struct sockaddr_in m_dest;
};

}  // namespace wpilibxrp
