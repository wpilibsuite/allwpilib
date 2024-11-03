// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/MecanumDriveWheelPositionsProto.h"

#include "kinematics.npb.h"

const pb_msgdesc_t* wpi::Protobuf<frc::MecanumDriveWheelPositions>::Message() {
  return get_wpi_proto_ProtobufMecanumDriveWheelPositions_msg();
}

std::optional<frc::MecanumDriveWheelPositions> wpi::Protobuf<
    frc::MecanumDriveWheelPositions>::Unpack(wpi::ProtoInputStream& stream) {
  wpi_proto_ProtobufMecanumDriveWheelPositions msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return frc::MecanumDriveWheelPositions{
      units::meter_t{msg.front_left},
      units::meter_t{msg.front_right},
      units::meter_t{msg.rear_left},
      units::meter_t{msg.rear_right},
  };
}

bool wpi::Protobuf<frc::MecanumDriveWheelPositions>::Pack(
    wpi::ProtoOutputStream& stream,
    const frc::MecanumDriveWheelPositions& value) {
  wpi_proto_ProtobufMecanumDriveWheelPositions msg{
      .front_left = value.frontLeft.value(),
      .front_right = value.frontRight.value(),
      .rear_left = value.rearLeft.value(),
      .rear_right = value.rearRight.value(),
  };
  return stream.Encode(msg);
}
