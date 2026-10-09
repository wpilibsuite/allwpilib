// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "wpi/halsim/ws_core/WSHalProviders.hpp"

namespace wpilibws {

/** Bridges onboard sensor WebSocket messages to the onboard IMU. */
class HALSimWSProviderIMU : public HALSimWSHalProvider {
 public:
  /**
   * Registers the onboard gyro and accelerometer providers.
   *
   * @param webRegisterFunc provider registration callback
   * @param gyroName firmware gyro device name
   */
  static void Initialize(WSRegisterFunc webRegisterFunc,
                         std::string_view gyroName);

  /**
   * Constructs a provider for an onboard sensor.
   *
   * @param key provider lookup key
   * @param type protocol device type (Gyro or Accel)
   * @param deviceId protocol device name
   */
  HALSimWSProviderIMU(std::string_view key, std::string_view type,
                      std::string_view deviceId);

  /**
   * Updates the onboard IMU from firmware sensor values.
   *
   * @param json sensor fields
   */
  void OnNetValueChanged(const wpi::util::json& json) override;

 protected:
  void RegisterCallbacks() override;
  void CancelCallbacks() override {}
};

}  // namespace wpilibws
