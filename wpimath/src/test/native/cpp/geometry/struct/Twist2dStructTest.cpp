// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Twist2d.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::Twist2d>;
const Twist2d kExpectedData{Twist2d{2.29_m, 35.04_m, 35.04_rad}};
}  // namespace

TEST_CASE("Twist2dStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  Twist2d unpacked_data = StructType::Unpack(buffer);

  CHECK(kExpectedData.dx.value() == unpacked_data.dx.value());
  CHECK(kExpectedData.dy.value() == unpacked_data.dy.value());
  CHECK(kExpectedData.dtheta.value() == unpacked_data.dtheta.value());
}
