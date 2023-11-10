// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/spline/struct/CubicHermiteSplineStruct.h"

frc::CubicHermiteSpline wpi::Struct<frc::CubicHermiteSpline>::Unpack(
    std::span<const uint8_t, kSize> data) {
  return frc::CubicHermiteSpline{wpi::UnpackStructArray<double, 0, 2>(data),
                                 wpi::UnpackStructArray<double, 16, 2>(data),
                                 wpi::UnpackStructArray<double, 32, 2>(data),
                                 wpi::UnpackStructArray<double, 48, 2>(data)};
}

void wpi::Struct<frc::CubicHermiteSpline>::Pack(
    std::span<uint8_t, kSize> data, const frc::CubicHermiteSpline& value) {
  wpi::PackStructArray<0, 2>(data, value.xInitialControlVector);
  wpi::PackStructArray<16, 2>(data, value.xFinalControlVector);
  wpi::PackStructArray<32, 2>(data, value.yInitialControlVector);
  wpi::PackStructArray<48, 2>(data, value.yFinalControlVector);
}
