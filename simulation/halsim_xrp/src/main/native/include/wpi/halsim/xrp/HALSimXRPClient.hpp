// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "wpi/halsim/ws_core/WSProviderContainer.hpp"
#include "wpi/halsim/ws_core/WSProvider_SimDevice.hpp"
#include "wpi/halsim/xrp/HALSimXRP.hpp"
#include "wpi/net/EventLoopRunner.hpp"

namespace wpilibxrp {

class HALSimXRPClient {
 public:
  HALSimXRPClient() = default;
  HALSimXRPClient(const HALSimXRPClient&) = delete;
  HALSimXRPClient& operator=(const HALSimXRPClient&) = delete;

  bool Initialize();

  wpilibws::ProviderContainer providers;
  wpilibws::HALSimWSProviderSimDevices simDevices{providers};
  wpi::net::EventLoopRunner runner;
  std::shared_ptr<HALSimXRP> simxrp;
};

}  // namespace wpilibxrp
