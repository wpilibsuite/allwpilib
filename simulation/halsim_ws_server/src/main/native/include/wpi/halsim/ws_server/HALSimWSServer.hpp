// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "wpi/halsim/ws_core/WSProviderContainer.hpp"
#include "wpi/halsim/ws_core/WSProvider_SimDevice.hpp"
#include "wpi/net/EventLoopRunner.hpp"

#include "wpi/halsim/ws_server/HALSimWeb.hpp"

namespace wpilibws {

class HALSimWSServer {
 public:
  HALSimWSServer() = default;
  HALSimWSServer(const HALSimWSServer&) = delete;
  HALSimWSServer& operator=(const HALSimWSServer&) = delete;

  bool Initialize();

  ProviderContainer providers;
  HALSimWSProviderSimDevices simDevices{providers};
  wpi::EventLoopRunner runner;
  std::shared_ptr<HALSimWeb> simWeb;
};

}  // namespace wpilibws
