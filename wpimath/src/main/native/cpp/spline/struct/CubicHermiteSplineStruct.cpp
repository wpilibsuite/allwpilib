// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/spline/struct/CubicHermiteSplineStruct.hpp"

namespace {
constexpr size_t kXInitialOff = 0;
constexpr size_t kXFinalOff = kXInitialOff + 2 * 8;
constexpr size_t kYInitialOff = kXFinalOff + 2 * 8;
constexpr size_t kYFinalOff = kYInitialOff + 2 * 8;
}  // namespace

wpi::math::CubicHermiteSpline wpi::util::Struct<
    wpi::math::CubicHermiteSpline>::Unpack(std::span<const uint8_t> data) {
  return wpi::math::CubicHermiteSpline{
      wpi::util::UnpackStructArray<double, kXInitialOff, 2>(data),
      wpi::util::UnpackStructArray<double, kXFinalOff, 2>(data),
      wpi::util::UnpackStructArray<double, kYInitialOff, 2>(data),
      wpi::util::UnpackStructArray<double, kYFinalOff, 2>(data)};
}

void wpi::util::Struct<wpi::math::CubicHermiteSpline>::Pack(
    std::span<uint8_t> data, const wpi::math::CubicHermiteSpline& value) {
  wpi::util::PackStructArray<kXInitialOff, 2>(
      data, value.GetInitialControlVector().x);
  wpi::util::PackStructArray<kXFinalOff, 2>(data,
                                            value.GetFinalControlVector().x);
  wpi::util::PackStructArray<kYInitialOff, 2>(
      data, value.GetInitialControlVector().y);
  wpi::util::PackStructArray<kYFinalOff, 2>(data,
                                            value.GetFinalControlVector().y);
}
