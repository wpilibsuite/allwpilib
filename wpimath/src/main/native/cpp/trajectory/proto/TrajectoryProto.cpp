// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/proto/TrajectoryProto.h"

#include <vector>

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "wpimath/protobuf/trajectory.npb.h"

std::optional<wpi::math::Trajectory> wpi::Protobuf<wpi::math::Trajectory>::Unpack(
    InputStream& stream) {
  wpi::StdVectorUnpackCallback<wpi::math::Trajectory::State, SIZE_MAX> states;
  wpi_proto_ProtobufTrajectory msg{
      .states = states.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::Trajectory{states.Vec()};
}

bool wpi::Protobuf<wpi::math::Trajectory>::Pack(
    OutputStream& stream, const wpi::math::Trajectory& value) {
  wpi::PackCallback<wpi::math::Trajectory::State> states{value.States()};
  wpi_proto_ProtobufTrajectory msg{
      .states = states.Callback(),
  };
  return stream.Encode(msg);
}
