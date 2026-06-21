// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Twist3d.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::Twist3d>;
const Twist3d kExpectedData{
    Twist3d{1.1_m, 2.29_m, 35.04_m, 0.174_rad, 19.1_rad, 4.4_rad}};
}  // namespace

TEST_CASE("Twist3dStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  Twist3d unpacked_data = StructType::Unpack(buffer);

  CHECK(kExpectedData.dx.value() == unpacked_data.dx.value());
  CHECK(kExpectedData.dy.value() == unpacked_data.dy.value());
  CHECK(kExpectedData.dz.value() == unpacked_data.dz.value());
  CHECK(kExpectedData.rx.value() == unpacked_data.rx.value());
  CHECK(kExpectedData.ry.value() == unpacked_data.ry.value());
  CHECK(kExpectedData.rz.value() == unpacked_data.rz.value());
}
