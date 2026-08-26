// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/shape/Rectangle2d.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::Rectangle2d>;
const Rectangle2d EXPECTED_DATA{
    Pose2d{Translation2d{0.191_m, 2.2_m}, Rotation2d{22.9_rad}}, 1.2_m, 2.3_m};
}  // namespace

TEST_CASE("Rectangle2dStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  Rectangle2d unpacked_data = StructType::Unpack(buffer);

  CHECK(EXPECTED_DATA.Center() == unpacked_data.Center());
  CHECK(EXPECTED_DATA.XWidth() == unpacked_data.XWidth());
  CHECK(EXPECTED_DATA.YWidth() == unpacked_data.YWidth());
}
