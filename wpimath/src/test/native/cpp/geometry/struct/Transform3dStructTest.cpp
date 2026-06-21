// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Transform3d.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::Transform3d>;
const Transform3d kExpectedData{
    Transform3d{Translation3d{0.3504_m, 22.9_m, 3.504_m},
                Rotation3d{Quaternion{0.3504, 35.04, 2.29, 0.3504}}}};
}  // namespace

TEST_CASE("Transform3dStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  Transform3d unpacked_data = StructType::Unpack(buffer);

  CHECK(kExpectedData.Translation() == unpacked_data.Translation());
  CHECK(kExpectedData.Rotation() == unpacked_data.Rotation());
}
