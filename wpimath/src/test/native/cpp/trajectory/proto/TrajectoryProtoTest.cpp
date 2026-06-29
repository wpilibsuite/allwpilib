// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/trajectory/Trajectory.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {

using ProtoType = wpi::util::Protobuf<wpi::math::Trajectory>;

const Trajectory kExpectedData =
    Trajectory{std::vector<wpi::math::Trajectory::State>{
        Trajectory::State{
            1.1_s, 2.2_mps, 3.3_mps_sq,
            Pose2d(Translation2d(1.1_m, 2.2_m), Rotation2d(2.2_rad)),
            wpi::units::curvature_t{6.6}},
        Trajectory::State{
            2.1_s, 2.2_mps, 3.3_mps_sq,
            Pose2d(Translation2d(2.1_m, 2.2_m), Rotation2d(2.2_rad)),
            wpi::units::curvature_t{6.6}},
        Trajectory::State{
            3.1_s, 2.2_mps, 3.3_mps_sq,
            Pose2d(Translation2d(3.1_m, 2.2_m), Rotation2d(2.2_rad)),
            wpi::units::curvature_t{6.6}}}};
}  // namespace

TEST_CASE("TrajectoryProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());
  CHECK(kExpectedData.States() == unpacked_data->States());
}
