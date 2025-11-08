// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "wpi/halsim/ws_core/WSProviderContainer.hpp"
#include "wpi/halsim/ws_core/WSProvider_SimDevice.hpp"
#include "wpi/net/EventLoopRunner.hpp"

#include "wpi/halsim/ws_client/HALSimWS.hpp"

namespace wpilibws {

class HALSimWSClient {
 public:
  HALSimWSClient() = default;
  HALSimWSClient(const HALSimWSClient&) = delete;
  HALSimWSClient& operator=(const HALSimWSClient&) = delete;

  bool Initialize();

  ProviderContainer providers;
  HALSimWSProviderSimDevices simDevices{providers};
  wpi::EventLoopRunner runner;
  std::shared_ptr<HALSimWS> simws;
};

}  // namespace wpilibws
