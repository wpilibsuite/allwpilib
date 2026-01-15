// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/proto/DifferentialDriveWheelVelocitiesProto.hpp"

#include "wpimath/protobuf/kinematics.npb.h"

std::optional<wpi::math::DifferentialDriveWheelVelocities> wpi::util::Protobuf<
    wpi::math::DifferentialDriveWheelVelocities>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufDifferentialDriveWheelVelocities msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::DifferentialDriveWheelVelocities{
      wpi::units::meters_per_second_t{msg.left},
      wpi::units::meters_per_second_t{msg.right},
  };
}

bool wpi::util::Protobuf<wpi::math::DifferentialDriveWheelVelocities>::Pack(
    OutputStream& stream,
    const wpi::math::DifferentialDriveWheelVelocities& value) {
  wpi_proto_ProtobufDifferentialDriveWheelVelocities msg{
      .left = value.left.value(),
      .right = value.right.value(),
  };
  return stream.Encode(msg);
}
