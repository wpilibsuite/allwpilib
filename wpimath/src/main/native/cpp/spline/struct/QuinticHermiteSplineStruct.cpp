// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/spline/struct/QuinticHermiteSplineStruct.h"

namespace {
constexpr size_t kXInitialOff = 0;
constexpr size_t kXFinalOff = kXInitialOff + 3 * 8;
constexpr size_t kYInitialOff = kXFinalOff + 3 * 8;
constexpr size_t kYFinalOff = kYInitialOff + 3 * 8;
}  // namespace

frc::QuinticHermiteSpline wpi::Struct<frc::QuinticHermiteSpline>::Unpack(
    std::span<const uint8_t> data) {
  return frc::QuinticHermiteSpline{
      wpi::UnpackStructArray<double, kXInitialOff, 3>(data),
      wpi::UnpackStructArray<double, kXFinalOff, 3>(data),
      wpi::UnpackStructArray<double, kYInitialOff, 3>(data),
      wpi::UnpackStructArray<double, kYFinalOff, 3>(data)};
}

void wpi::Struct<frc::QuinticHermiteSpline>::Pack(
    std::span<uint8_t> data, const frc::QuinticHermiteSpline& value) {
  wpi::PackStructArray<kXInitialOff, 3>(data,
                                        value.GetInitialControlVector().x);
  wpi::PackStructArray<kXFinalOff, 3>(data, value.GetFinalControlVector().x);
  wpi::PackStructArray<kYInitialOff, 3>(data,
                                        value.GetInitialControlVector().y);
  wpi::PackStructArray<kYFinalOff, 3>(data, value.GetFinalControlVector().y);
}
