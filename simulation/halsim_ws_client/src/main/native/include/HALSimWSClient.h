// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <WSProviderContainer.h>
#include <WSProvider_SimDevice.h>
#include <wpi/EventLoopRunner.h>

#include "HALSimWS.h"

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
