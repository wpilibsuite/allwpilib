// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/apriltag/AprilTag.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::apriltag;

namespace {

const AprilTag kExpectedData =
    AprilTag{3, wpi::math::Pose3d{0_m, 1_m, 0_m,
                                  wpi::math::Rotation3d{0_deg, 0_deg, 0_deg}}};
}  // namespace

TEST_CASE("AprilTagProtoTest Roundtrip", "[apriltag]") {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  REQUIRE(unpacked_data->ID == kExpectedData.ID);
  REQUIRE(unpacked_data->pose == kExpectedData.pose);
}
