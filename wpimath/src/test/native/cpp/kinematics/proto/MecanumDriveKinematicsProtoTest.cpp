// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/MecanumDriveKinematics.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {

const MecanumDriveKinematics EXPECTED_DATA = MecanumDriveKinematics{
    Translation2d{19.1_m, 2.2_m}, Translation2d{35.04_m, 1.91_m},
    Translation2d{1.74_m, 3.504_m}, Translation2d{3.504_m, 1.91_m}};
}  // namespace

TEST_CASE("MecanumDriveKinematicsProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(EXPECTED_DATA.GetFrontLeft() == unpacked_data->GetFrontLeft());
  CHECK(EXPECTED_DATA.GetFrontRight() == unpacked_data->GetFrontRight());
  CHECK(EXPECTED_DATA.GetRearLeft() == unpacked_data->GetRearLeft());
  CHECK(EXPECTED_DATA.GetRearRight() == unpacked_data->GetRearRight());
}
