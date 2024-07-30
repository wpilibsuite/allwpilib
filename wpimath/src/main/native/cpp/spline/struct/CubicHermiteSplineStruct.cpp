// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/spline/struct/CubicHermiteSplineStruct.h"

namespace {
constexpr size_t kXInitialOff = 0;
constexpr size_t kXFinalOff = kXInitialOff + 2 * 8;
constexpr size_t kYInitialOff = kXFinalOff + 2 * 8;
constexpr size_t kYFinalOff = kYInitialOff + 2 * 8;
}  // namespace

frc::CubicHermiteSpline wpi::Struct<frc::CubicHermiteSpline>::Unpack(
    std::span<const uint8_t> data) {
  return frc::CubicHermiteSpline{
      wpi::UnpackStructArray<double, kXInitialOff, 2>(data),
      wpi::UnpackStructArray<double, kXFinalOff, 2>(data),
      wpi::UnpackStructArray<double, kYInitialOff, 2>(data),
      wpi::UnpackStructArray<double, kYFinalOff, 2>(data)};
}

void wpi::Struct<frc::CubicHermiteSpline>::Pack(
    std::span<uint8_t> data, const frc::CubicHermiteSpline& value) {
  wpi::PackStructArray<kXInitialOff, 2>(data,
                                        value.GetInitialControlVector().x);
  wpi::PackStructArray<kXFinalOff, 2>(data, value.GetFinalControlVector().x);
  wpi::PackStructArray<kYInitialOff, 2>(data,
                                        value.GetInitialControlVector().y);
  wpi::PackStructArray<kYFinalOff, 2>(data, value.GetFinalControlVector().y);
}
