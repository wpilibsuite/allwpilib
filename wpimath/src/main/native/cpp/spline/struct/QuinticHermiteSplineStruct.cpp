// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/spline/struct/QuinticHermiteSplineStruct.hpp"

namespace {
constexpr size_t kXInitialOff = 0;
constexpr size_t kXFinalOff = kXInitialOff + 3 * 8;
constexpr size_t kYInitialOff = kXFinalOff + 3 * 8;
constexpr size_t kYFinalOff = kYInitialOff + 3 * 8;
}  // namespace

wpi::math::QuinticHermiteSpline wpi::util::Struct<wpi::math::QuinticHermiteSpline>::Unpack(
    std::span<const uint8_t> data) {
  return wpi::math::QuinticHermiteSpline{
      wpi::util::UnpackStructArray<double, kXInitialOff, 3>(data),
      wpi::util::UnpackStructArray<double, kXFinalOff, 3>(data),
      wpi::util::UnpackStructArray<double, kYInitialOff, 3>(data),
      wpi::util::UnpackStructArray<double, kYFinalOff, 3>(data)};
}

void wpi::util::Struct<wpi::math::QuinticHermiteSpline>::Pack(
    std::span<uint8_t> data, const wpi::math::QuinticHermiteSpline& value) {
  wpi::util::PackStructArray<kXInitialOff, 3>(data,
                                        value.GetInitialControlVector().x);
  wpi::util::PackStructArray<kXFinalOff, 3>(data, value.GetFinalControlVector().x);
  wpi::util::PackStructArray<kYInitialOff, 3>(data,
                                        value.GetInitialControlVector().y);
  wpi::util::PackStructArray<kYFinalOff, 3>(data, value.GetFinalControlVector().y);
}
