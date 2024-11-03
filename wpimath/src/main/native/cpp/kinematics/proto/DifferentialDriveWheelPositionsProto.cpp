// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/DifferentialDriveWheelPositionsProto.h"

#include "kinematics.npb.h"

const pb_msgdesc_t*
wpi::Protobuf<frc::DifferentialDriveWheelPositions>::Message() {
  return get_wpi_proto_ProtobufDifferentialDriveWheelPositions_msg();
}

std::optional<frc::DifferentialDriveWheelPositions>
wpi::Protobuf<frc::DifferentialDriveWheelPositions>::Unpack(
    wpi::ProtoInputStream& stream) {
  wpi_proto_ProtobufDifferentialDriveWheelPositions msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return frc::DifferentialDriveWheelPositions{
      units::meter_t{msg.left},
      units::meter_t{msg.right},
  };
}

bool wpi::Protobuf<frc::DifferentialDriveWheelPositions>::Pack(
    wpi::ProtoOutputStream& stream,
    const frc::DifferentialDriveWheelPositions& value) {
  wpi_proto_ProtobufDifferentialDriveWheelPositions msg{
      .left = value.left.value(),
      .right = value.right.value(),
  };
  return stream.Encode(msg);
}
