// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Quaternion.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::Quaternion>;
const Quaternion kExpectedData{Quaternion{1.1, 0.191, 35.04, 19.1}};
}  // namespace

TEST_CASE("QuaternionStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  Quaternion unpacked_data = StructType::Unpack(buffer);

  CHECK(kExpectedData.W() == unpacked_data.W());
  CHECK(kExpectedData.X() == unpacked_data.X());
  CHECK(kExpectedData.Y() == unpacked_data.Y());
  CHECK(kExpectedData.Z() == unpacked_data.Z());
}
