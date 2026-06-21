// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Pose3d.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::Pose3d>;
const Pose3d kExpectedData{
    Pose3d{Translation3d{1.1_m, 2.2_m, 1.1_m},
           Rotation3d{Quaternion{1.91, 0.3504, 3.3, 1.74}}}};
}  // namespace

TEST_CASE("Pose3dStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  Pose3d unpacked_data = StructType::Unpack(buffer);

  CHECK(kExpectedData.Translation() == unpacked_data.Translation());
  CHECK(kExpectedData.Rotation() == unpacked_data.Rotation());
}
