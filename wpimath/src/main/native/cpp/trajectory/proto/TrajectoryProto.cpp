// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/trajectory/proto/TrajectoryProto.h"

#include <vector>

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "trajectory.npb.h"

const pb_msgdesc_t* wpi::Protobuf<frc::Trajectory>::Message() {
  return get_wpi_proto_ProtobufTrajectory_msg();
}

std::optional<frc::Trajectory> wpi::Protobuf<frc::Trajectory>::Unpack(
    wpi::ProtoInputStream& stream) {
  wpi::StdVectorUnpackCallback<frc::Trajectory::State, SIZE_MAX> states;
  wpi_proto_ProtobufTrajectory msg{
      .states = states.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  return frc::Trajectory{states.Vec()};
}

bool wpi::Protobuf<frc::Trajectory>::Pack(wpi::ProtoOutputStream& stream,
                                          const frc::Trajectory& value) {
  wpi::PackCallback<frc::Trajectory::State> states{value.States()};
  wpi_proto_ProtobufTrajectory msg{
      .states = states.Callback(),
  };
  return stream.Encode(msg);
}
