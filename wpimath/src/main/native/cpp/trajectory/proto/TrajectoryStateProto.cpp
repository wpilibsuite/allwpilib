// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <utility>

#include <wpi/math/trajectory/proto/TrajectoryStateProto.hpp>
#include <wpi/protobuf/ProtobufCallbacks.h>

#include "wpimath/protobuf/trajectory.npb.hpp"

std::optional<wpi::math::Trajectory::State>
wpi::Protobuf<wpi::math::Trajectory::State>::Unpack(InputStream& stream) {
  wpi::UnpackCallback<wpi::math::Pose2d> pose;
  wpi_proto_ProtobufTrajectoryState msg;
  msg.pose = pose.Callback();

  if (!stream.Decode(msg)) {
    return {};
  }

  auto ipose = pose.Items();

  if (ipose.empty()) {
    return {};
  }

  return wpi::math::Trajectory::State{
      units::second_t{msg.time},
      units::meters_per_second_t{msg.velocity},
      units::meters_per_second_squared_t{msg.acceleration},
      std::move(ipose[0]),
      units::curvature_t{msg.curvature},
  };
}

bool wpi::Protobuf<wpi::math::Trajectory::State>::Pack(
    OutputStream& stream, const wpi::math::Trajectory::State& value) {
  wpi::PackCallback pose{&value.pose};
  wpi_proto_ProtobufTrajectoryState msg{
      .time = value.t.value(),
      .velocity = value.velocity.value(),
      .acceleration = value.acceleration.value(),
      .pose = pose.Callback(),
      .curvature = value.curvature.value(),
  };
  return stream.Encode(msg);
}
