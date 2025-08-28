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

units::radian_t OnboardIMU::GetYawNoOffset() {
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

units::radian_t OnboardIMU::GetYaw() {
  return GetYawNoOffset() - m_yawOffset;
}

void OnboardIMU::ResetYaw() {
  m_yawOffset = GetYawNoOffset();
}

wpi::math::Rotation2d OnboardIMU::GetRotation2d() {
  return wpi::math::Rotation2d{GetYaw()};
}

wpi::math::Rotation3d OnboardIMU::GetRotation3d() {
  return wpi::math::Rotation3d{GetQuaternion()};
}

wpi::math::Quaternion OnboardIMU::GetQuaternion() {
  HAL_Quaternion val;
  int32_t status = 0;
  HAL_GetIMUQuaternion(&val, &status);
  FRC_CheckErrorStatus(status, "Onboard IMU");
  return wpi::math::Quaternion{val.w, val.x, val.y, val.z};
}

units::radian_t OnboardIMU::GetAngleX() {
  HAL_EulerAngles3d val;
  int32_t status = 0;
  switch (m_mountOrientation) {
    case kFlat:
      HAL_GetIMUEulerAnglesFlat(&val, &status);
      break;
    case kLandscape:
      HAL_GetIMUEulerAnglesLandscape(&val, &status);
      break;
    case kPortrait:
      HAL_GetIMUEulerAnglesPortrait(&val, &status);
      break;
  }
  FRC_CheckErrorStatus(status, "Onboard IMU");
  return units::radian_t{val.x};
}

units::radian_t OnboardIMU::GetAngleY() {
  HAL_EulerAngles3d val;
  int32_t status = 0;
  switch (m_mountOrientation) {
    case kFlat:
      HAL_GetIMUEulerAnglesFlat(&val, &status);
      break;
    case kLandscape:
      HAL_GetIMUEulerAnglesLandscape(&val, &status);
      break;
    case kPortrait:
      HAL_GetIMUEulerAnglesPortrait(&val, &status);
      break;
  }
  FRC_CheckErrorStatus(status, "Onboard IMU");
  return units::radian_t{val.y};
}

units::radian_t OnboardIMU::GetAngleZ() {
  HAL_EulerAngles3d val;
  int32_t status = 0;
  switch (m_mountOrientation) {
    case kFlat:
      HAL_GetIMUEulerAnglesFlat(&val, &status);
      break;
    case kLandscape:
      HAL_GetIMUEulerAnglesLandscape(&val, &status);
      break;
    case kPortrait:
      HAL_GetIMUEulerAnglesPortrait(&val, &status);
      break;
  }
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
