// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/trajectory/proto/TrajectoryProto.h"

#include <vector>

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "wpimath/protobuf/trajectory.npb.h"

std::optional<frc::Trajectory> wpi::Protobuf<frc::Trajectory>::Unpack(
    InputStream& stream) {
  wpi::StdVectorUnpackCallback<frc::Trajectory::State, SIZE_MAX> states;
  wpi_proto_ProtobufTrajectory msg{
      .states = states.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  return frc::Trajectory{states.Vec()};
}

bool wpi::Protobuf<frc::Trajectory>::Pack(OutputStream& stream,
                                          const frc::Trajectory& value) {
  wpi::PackCallback<frc::Trajectory::State> states{value.States()};
  wpi_proto_ProtobufTrajectory msg{
      .states = states.Callback(),
  };
  return stream.Encode(msg);
}
