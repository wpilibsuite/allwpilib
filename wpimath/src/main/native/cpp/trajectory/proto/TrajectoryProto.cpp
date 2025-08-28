// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/trajectory/proto/TrajectoryProto.h"

#include <vector>

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "wpimath/protobuf/trajectory.npb.h"

std::optional<wpimath::Trajectory> wpi::Protobuf<wpimath::Trajectory>::Unpack(
    InputStream& stream) {
  wpi::StdVectorUnpackCallback<wpimath::Trajectory::State, SIZE_MAX> states;
  wpi_proto_ProtobufTrajectory msg{
      .states = states.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpimath::Trajectory{states.Vec()};
}

bool wpi::Protobuf<wpimath::Trajectory>::Pack(
    OutputStream& stream, const wpimath::Trajectory& value) {
  wpi::PackCallback<wpimath::Trajectory::State> states{value.States()};
  wpi_proto_ProtobufTrajectory msg{
      .states = states.Callback(),
  };
  return stream.Encode(msg);
}
