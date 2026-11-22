// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/OnboardIMUSim.hpp"

#include "wpi/hal/simulation/IMUData.h"

namespace wpi::sim {

void OnboardIMUSim::SetAngleX(wpi::units::radian_t angle) {
  HALSIM_SetIMUAngleX(angle.to<double>());
}
void OnboardIMUSim::SetAngleY(wpi::units::radian_t angle) {
  HALSIM_SetIMUAngleY(angle.to<double>());
}
void OnboardIMUSim::SetAngleZ(wpi::units::radian_t angle) {
  HALSIM_SetIMUAngleZ(angle.to<double>());
}

void OnboardIMUSim::SetGyroRateX(wpi::units::radians_per_second_t rate) {
  HALSIM_SetIMUGyroRateX(rate.to<double>());
}

void OnboardIMUSim::SetGyroRateY(wpi::units::radians_per_second_t rate) {
  HALSIM_SetIMUGyroRateY(rate.to<double>());
}

void OnboardIMUSim::SetGyroRateZ(wpi::units::radians_per_second_t rate) {
  HALSIM_SetIMUGyroRateZ(rate.to<double>());
}

void OnboardIMUSim::SetAccelX(wpi::units::meters_per_second_squared_t accel) {
  HALSIM_SetIMUAccelX(accel.to<double>());
}

void OnboardIMUSim::SetAccelY(wpi::units::meters_per_second_squared_t accel) {
  HALSIM_SetIMUAccelY(accel.to<double>());
}

void OnboardIMUSim::SetAccelZ(wpi::units::meters_per_second_squared_t accel) {
  HALSIM_SetIMUAccelZ(accel.to<double>());
}

void OnboardIMUSim::SetYaw(wpi::units::radian_t angle) {
  HALSIM_SetIMUYaw(angle.to<double>());
}

}  // namespace wpi::sim
