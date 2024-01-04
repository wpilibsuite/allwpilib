// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/spline/struct/QuinticHermiteSplineStruct.h"

frc::QuinticHermiteSpline wpi::Struct<frc::QuinticHermiteSpline>::Unpack(
    std::span<const uint8_t> data) {
  return frc::QuinticHermiteSpline{wpi::UnpackStructArray<double, 0, 3>(data),
                                   wpi::UnpackStructArray<double, 24, 3>(data),
                                   wpi::UnpackStructArray<double, 48, 3>(data),
                                   wpi::UnpackStructArray<double, 72, 3>(data)};
}

void wpi::Struct<frc::QuinticHermiteSpline>::Pack(
    std::span<uint8_t> data, const frc::QuinticHermiteSpline& value) {
  wpi::PackStructArray<0, 3>(data, value.GetInitialControlVector().x);
  wpi::PackStructArray<24, 3>(data, value.GetFinalControlVector().x);
  wpi::PackStructArray<48, 3>(data, value.GetInitialControlVector().y);
  wpi::PackStructArray<72, 3>(data, value.GetFinalControlVector().y);
}
