// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/spline/struct/CubicHermiteSplineStruct.hpp"

namespace {
constexpr size_t X_INITIAL_OFF = 0;
constexpr size_t X_FINAL_OFF = X_INITIAL_OFF + 2 * 8;
constexpr size_t Y_INITIAL_OFF = X_FINAL_OFF + 2 * 8;
constexpr size_t Y_FINAL_OFF = Y_INITIAL_OFF + 2 * 8;
}  // namespace

wpi::math::CubicHermiteSpline wpi::util::Struct<
    wpi::math::CubicHermiteSpline>::Unpack(std::span<const uint8_t> data) {
  return wpi::math::CubicHermiteSpline{
      wpi::util::UnpackStructArray<double, X_INITIAL_OFF, 2>(data),
      wpi::util::UnpackStructArray<double, X_FINAL_OFF, 2>(data),
      wpi::util::UnpackStructArray<double, Y_INITIAL_OFF, 2>(data),
      wpi::util::UnpackStructArray<double, Y_FINAL_OFF, 2>(data)};
}

void wpi::util::Struct<wpi::math::CubicHermiteSpline>::Pack(
    std::span<uint8_t> data, const wpi::math::CubicHermiteSpline& value) {
  wpi::util::PackStructArray<X_INITIAL_OFF, 2>(
      data, value.GetInitialControlVector().x);
  wpi::util::PackStructArray<X_FINAL_OFF, 2>(data,
                                             value.GetFinalControlVector().x);
  wpi::util::PackStructArray<Y_INITIAL_OFF, 2>(
      data, value.GetInitialControlVector().y);
  wpi::util::PackStructArray<Y_FINAL_OFF, 2>(data,
                                             value.GetFinalControlVector().y);
}
