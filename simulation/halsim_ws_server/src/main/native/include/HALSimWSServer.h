// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <WSProviderContainer.h>
#include <WSProvider_SimDevice.h>
#include <wpi/EventLoopRunner.h>

#include "HALSimWeb.h"

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
