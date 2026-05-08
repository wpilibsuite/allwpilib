// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_velocity.hpp"

namespace wpi::sim {

class OnboardIMUSim {
 public:
  void SetAngleX(wpi::units::radian_t angle);
  void SetAngleY(wpi::units::radian_t angle);
  void SetAngleZ(wpi::units::radian_t angle);

  void SetGyroRateX(wpi::units::radians_per_second_t rate);
  void SetGyroRateY(wpi::units::radians_per_second_t rate);
  void SetGyroRateZ(wpi::units::radians_per_second_t rate);

  void SetAccelX(wpi::units::meters_per_second_squared_t accel);
  void SetAccelY(wpi::units::meters_per_second_squared_t accel);
  void SetAccelZ(wpi::units::meters_per_second_squared_t accel);

  void SetYaw(wpi::units::radian_t angle);
};

}  // namespace wpi::sim
