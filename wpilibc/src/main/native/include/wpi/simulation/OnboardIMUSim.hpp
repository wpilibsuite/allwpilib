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
  void SetAngleX(wpi::units::radians<> angle);
  void SetAngleY(wpi::units::radians<> angle);
  void SetAngleZ(wpi::units::radians<> angle);

  void SetGyroRateX(wpi::units::radians_per_second<> rate);
  void SetGyroRateY(wpi::units::radians_per_second<> rate);
  void SetGyroRateZ(wpi::units::radians_per_second<> rate);

  void SetAccelX(wpi::units::meters_per_second_squared<> accel);
  void SetAccelY(wpi::units::meters_per_second_squared<> accel);
  void SetAccelZ(wpi::units::meters_per_second_squared<> accel);

  void SetYaw(wpi::units::radians<> angle);
};

}  // namespace wpi::sim
