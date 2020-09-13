/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
