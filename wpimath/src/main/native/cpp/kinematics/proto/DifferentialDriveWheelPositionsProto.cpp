// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/math/kinematics/proto/DifferentialDriveWheelPositionsProto.h>

#include "wpimath/protobuf/kinematics.npb.h"

std::optional<wpi::math::DifferentialDriveWheelPositions> wpi::Protobuf<
    wpi::math::DifferentialDriveWheelPositions>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufDifferentialDriveWheelPositions msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::DifferentialDriveWheelPositions{
      units::meter_t{msg.left},
      units::meter_t{msg.right},
  };
}

bool wpi::Protobuf<wpi::math::DifferentialDriveWheelPositions>::Pack(
    OutputStream& stream,
    const wpi::math::DifferentialDriveWheelPositions& value) {
  wpi_proto_ProtobufDifferentialDriveWheelPositions msg{
      .left = value.left.value(),
      .right = value.right.value(),
  };
  return stream.Encode(msg);
}
