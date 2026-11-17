// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Translation2d.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::Translation2d>;
const Translation2d EXPECTED_DATA{Translation2d{3.504_m, 22.9_m}};
}  // namespace

TEST_CASE("Translation2dStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  Translation2d unpacked_data = StructType::Unpack(buffer);

  CHECK(EXPECTED_DATA.X() == unpacked_data.X());
  CHECK(EXPECTED_DATA.Y() == unpacked_data.Y());
}
