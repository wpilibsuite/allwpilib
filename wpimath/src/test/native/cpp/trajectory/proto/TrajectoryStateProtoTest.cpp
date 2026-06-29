// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/trajectory/Trajectory.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {

using ProtoType = wpi::util::Protobuf<wpi::math::Trajectory::State>;

const Trajectory::State kExpectedData = Trajectory::State{
    1.91_s, 4.4_mps, 17.4_mps_sq,
    Pose2d{Translation2d{1.74_m, 19.1_m}, Rotation2d{22.9_rad}},
    wpi::units::curvature_t{0.174}};
}  // namespace

TEST_CASE("TrajectoryStateProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(kExpectedData.t.value() == unpacked_data->t.value());
  CHECK(kExpectedData.velocity.value() == unpacked_data->velocity.value());
  CHECK(kExpectedData.acceleration.value() ==
        unpacked_data->acceleration.value());
  CHECK(kExpectedData.pose == unpacked_data->pose);
  CHECK(kExpectedData.curvature.value() == unpacked_data->curvature.value());
}
