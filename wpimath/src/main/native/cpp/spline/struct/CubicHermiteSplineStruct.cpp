// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/spline/struct/CubicHermiteSplineStruct.h"

namespace {
constexpr size_t X_INITIAL_OFF = 0;
constexpr size_t X_FINAL_OFF = X_INITIAL_OFF + 2 * 8;
constexpr size_t Y_INITIAL_OFF = X_FINAL_OFF + 2 * 8;
constexpr size_t Y_FINAL_OFF = Y_INITIAL_OFF + 2 * 8;
}  // namespace

frc::CubicHermiteSpline wpi::Struct<frc::CubicHermiteSpline>::Unpack(
    std::span<const uint8_t> data) {
  return frc::CubicHermiteSpline{
      wpi::UnpackStructArray<double, X_INITIAL_OFF, 2>(data),
      wpi::UnpackStructArray<double, X_FINAL_OFF, 2>(data),
      wpi::UnpackStructArray<double, Y_INITIAL_OFF, 2>(data),
      wpi::UnpackStructArray<double, Y_FINAL_OFF, 2>(data)};
}

void wpi::Struct<frc::CubicHermiteSpline>::Pack(
    std::span<uint8_t> data, const frc::CubicHermiteSpline& value) {
  wpi::PackStructArray<X_INITIAL_OFF, 2>(data,
                                        value.GetInitialControlVector().x);
  wpi::PackStructArray<X_FINAL_OFF, 2>(data, value.GetFinalControlVector().x);
  wpi::PackStructArray<Y_INITIAL_OFF, 2>(data,
                                        value.GetInitialControlVector().y);
  wpi::PackStructArray<Y_FINAL_OFF, 2>(data, value.GetFinalControlVector().y);
}
