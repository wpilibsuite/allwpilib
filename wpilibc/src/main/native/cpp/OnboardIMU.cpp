// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/OnboardIMU.h"

#include <hal/IMU.h>

#include "frc/Errors.h"

using namespace frc;

OnboardIMU::OnboardIMU(MountOrientation mountOrientation)
    : m_mountOrientation{mountOrientation} {
  // TODO: usage reporting
}

units::radian_t OnboardIMU::GetYaw() {
  int64_t timestamp;
  double val;
  switch (m_mountOrientation) {
    case kFlat:
      val = HAL_GetIMUYawFlat(&timestamp);
      break;
    case kLandscape:
      val = HAL_GetIMUYawLandscape(&timestamp);
      break;
    case kPortrait:
      val = HAL_GetIMUYawPortrait(&timestamp);
      break;
    default:
      val = 0;
  }
  return units::radian_t{val};
}

Rotation2d OnboardIMU::GetRotation2d() {
  return Rotation2d{GetYaw()};
}

Rotation3d OnboardIMU::GetRotation3d() {
  return Rotation3d{GetQuaternion()};
}

Quaternion OnboardIMU::GetQuaternion() {
  HAL_Quaternion val;
  int32_t status = 0;
  HAL_GetIMUQuaternion(&val, &status);
  FRC_CheckErrorStatus(status, "Onboard IMU");
  return Quaternion{val.w, val.x, val.y, val.z};
}

units::radian_t GetAngleX() {
  HAL_EulerAngles3d val;
  int32_t status = 0;
  HAL_GetIMUEulerAngles(&val, &status);
  FRC_CheckErrorStatus(status, "Onboard IMU");
  return units::radian_t{val.x};
}

units::radian_t GetAngleY() {
  HAL_EulerAngles3d val;
  int32_t status = 0;
  HAL_GetIMUEulerAngles(&val, &status);
  FRC_CheckErrorStatus(status, "Onboard IMU");
  return units::radian_t{val.y};
}

units::radian_t GetAngleZ() {
  HAL_EulerAngles3d val;
  int32_t status = 0;
  HAL_GetIMUEulerAngles(&val, &status);
  FRC_CheckErrorStatus(status, "Onboard IMU");
  return units::radian_t{val.z};
}

units::radians_per_second_t OnboardIMU::GetGyroRateX() {
  HAL_GyroRate3d val;
  int32_t status = 0;
  HAL_GetIMUGyroRates(&val, &status);
  FRC_CheckErrorStatus(status, "Onboard IMU");
  return units::radians_per_second_t{val.x};
}

units::radians_per_second_t OnboardIMU::GetGyroRateY() {
  HAL_GyroRate3d val;
  int32_t status = 0;
  HAL_GetIMUGyroRates(&val, &status);
  FRC_CheckErrorStatus(status, "Onboard IMU");
  return units::radians_per_second_t{val.y};
}

units::radians_per_second_t OnboardIMU::GetGyroRateZ() {
  HAL_GyroRate3d val;
  int32_t status = 0;
  HAL_GetIMUGyroRates(&val, &status);
  FRC_CheckErrorStatus(status, "Onboard IMU");
  return units::radians_per_second_t{val.z};
}

units::meters_per_second_squared_t OnboardIMU::GetAccelX() {
  HAL_Acceleration3d val;
  int32_t status = 0;
  HAL_GetIMUAcceleration(&val, &status);
  FRC_CheckErrorStatus(status, "Onboard IMU");
  return units::meters_per_second_squared_t{val.x};
}

units::meters_per_second_squared_t OnboardIMU::GetAccelY() {
  HAL_Acceleration3d val;
  int32_t status = 0;
  HAL_GetIMUAcceleration(&val, &status);
  FRC_CheckErrorStatus(status, "Onboard IMU");
  return units::meters_per_second_squared_t{val.x};
}

units::meters_per_second_squared_t OnboardIMU::GetAccelZ() {
  HAL_Acceleration3d val;
  int32_t status = 0;
  HAL_GetIMUAcceleration(&val, &status);
  FRC_CheckErrorStatus(status, "Onboard IMU");
  return units::meters_per_second_squared_t{val.x};
}
