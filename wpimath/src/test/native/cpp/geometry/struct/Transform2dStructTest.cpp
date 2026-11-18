// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Transform2d.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::Transform2d>;
const Transform2d EXPECTED_DATA{
    Transform2d{Translation2d{0.191_m, 2.2_m}, Rotation2d{4.4_rad}}};
}  // namespace

TEST_CASE("Transform2dStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  Transform2d unpacked_data = StructType::Unpack(buffer);

  CHECK(EXPECTED_DATA.Translation() == unpacked_data.Translation());
  CHECK(EXPECTED_DATA.Rotation() == unpacked_data.Rotation());
}
