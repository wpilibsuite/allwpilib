// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Translation3d.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::Translation3d>;
const Translation3d kExpectedData{Translation3d{35.04_m, 22.9_m, 3.504_m}};
}  // namespace

TEST_CASE("Translation3dStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  Translation3d unpacked_data = StructType::Unpack(buffer);

  CHECK(kExpectedData.X() == unpacked_data.X());
  CHECK(kExpectedData.Y() == unpacked_data.Y());
  CHECK(kExpectedData.Z() == unpacked_data.Z());
}
