// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Transform3d.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {

const Transform3d EXPECTED_DATA =
    Transform3d{Translation3d{0.3504_m, 22.9_m, 3.504_m},
                Rotation3d{Quaternion{0.3504, 35.04, 2.29, 0.3504}}};
}  // namespace

TEST_CASE("Transform3dProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(EXPECTED_DATA.Translation() == unpacked_data->Translation());
  CHECK(EXPECTED_DATA.Rotation() == unpacked_data->Rotation());
}
