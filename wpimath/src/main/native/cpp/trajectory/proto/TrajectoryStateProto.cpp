// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/trajectory/proto/TrajectoryStateProto.h"

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "trajectory.npb.h"

const pb_msgdesc_t* wpi::Protobuf<frc::Trajectory::State>::Message() {
  return get_wpi_proto_ProtobufTrajectoryState_msg();
}

std::optional<frc::Trajectory::State>
wpi::Protobuf<frc::Trajectory::State>::Unpack(wpi::ProtoInputStream& stream) {
  wpi::UnpackCallback<frc::Pose2d> pose;
  wpi_proto_ProtobufTrajectoryState msg;
  msg.pose = pose.Callback();

  if (!stream.DecodeNoInit(msg)) {
    return {};
  }

  auto ipose = pose.Items();

  if (ipose.empty()) {
    return {};
  }

  return frc::Trajectory::State{
      units::second_t{msg.time},
      units::meters_per_second_t{msg.velocity},
      units::meters_per_second_squared_t{msg.acceleration},
      std::move(ipose[0]),
      units::curvature_t{msg.curvature},
  };
}

bool wpi::Protobuf<frc::Trajectory::State>::Pack(
    wpi::ProtoOutputStream& stream, const frc::Trajectory::State& value) {
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
