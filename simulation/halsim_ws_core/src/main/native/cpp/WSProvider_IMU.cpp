// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/ws_core/WSProvider_IMU.hpp"

#include <format>
#include <memory>
#include <numbers>

#include "wpi/hal/simulation/IMUData.h"

namespace wpilibws {

void HALSimWSProviderIMU::Initialize(WSRegisterFunc webRegisterFunc,
                                     std::string_view gyroName) {
  auto gyroKey = std::format("Gyro/{}", gyroName);
  webRegisterFunc(gyroKey, std::make_shared<HALSimWSProviderIMU>(
                               gyroKey, "Gyro", gyroName));
  webRegisterFunc("Accel/BuiltInAccel",
                  std::make_shared<HALSimWSProviderIMU>(
                      "Accel/BuiltInAccel", "Accel", "BuiltInAccel"));
}

HALSimWSProviderIMU::HALSimWSProviderIMU(std::string_view key,
                                         std::string_view type,
                                         std::string_view deviceId)
    : HALSimWSHalProvider(key, type) {
  m_deviceId = deviceId;
}

void HALSimWSProviderIMU::RegisterCallbacks() {
  // Romi only streams sensors after receiving their initialization messages.
  auto data = wpi::util::json::object("<init", true);
  if (m_type == "Accel") {
    data["<range"] = 8;
  }
  ProcessHalCallback(data);
}

void HALSimWSProviderIMU::OnNetValueChanged(const wpi::util::json& json) {
  constexpr double DEGREES_TO_RADIANS = std::numbers::pi / 180.0;
  constexpr double STANDARD_GRAVITY = 9.80665;

  if (m_type == "Accel") {
    if (auto val = json.lookup(">x"); val && val->is_number()) {
      HALSIM_SetIMUAccelX(val->get_number() * STANDARD_GRAVITY);
    }
    if (auto val = json.lookup(">y"); val && val->is_number()) {
      HALSIM_SetIMUAccelY(val->get_number() * STANDARD_GRAVITY);
    }
    if (auto val = json.lookup(">z"); val && val->is_number()) {
      HALSIM_SetIMUAccelZ(val->get_number() * STANDARD_GRAVITY);
    }
  } else {
    if (auto val = json.lookup(">angle_x"); val && val->is_number()) {
      HALSIM_SetIMUAngleX(val->get_number() * DEGREES_TO_RADIANS);
    }
    if (auto val = json.lookup(">angle_y"); val && val->is_number()) {
      HALSIM_SetIMUAngleY(val->get_number() * DEGREES_TO_RADIANS);
    }
    if (auto val = json.lookup(">angle_z"); val && val->is_number()) {
      double angle = val->get_number() * DEGREES_TO_RADIANS;
      HALSIM_SetIMUAngleZ(angle);
      HALSIM_SetIMUYaw(angle);
    }
    if (auto val = json.lookup(">rate_x"); val && val->is_number()) {
      HALSIM_SetIMUGyroRateX(val->get_number() * DEGREES_TO_RADIANS);
    }
    if (auto val = json.lookup(">rate_y"); val && val->is_number()) {
      HALSIM_SetIMUGyroRateY(val->get_number() * DEGREES_TO_RADIANS);
    }
    if (auto val = json.lookup(">rate_z"); val && val->is_number()) {
      HALSIM_SetIMUGyroRateZ(val->get_number() * DEGREES_TO_RADIANS);
    }
  }
}

}  // namespace wpilibws
