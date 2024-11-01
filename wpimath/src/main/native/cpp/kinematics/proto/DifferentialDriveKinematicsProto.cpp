// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/DifferentialDriveKinematicsProto.h"

#include "kinematics.npb.h"

const pb_msgdesc_t* wpi::Protobuf<frc::DifferentialDriveKinematics>::Message() {
  return get_wpi_proto_ProtobufDifferentialDriveKinematics_msg();
}

std::optional<frc::DifferentialDriveKinematics> wpi::Protobuf<
    frc::DifferentialDriveKinematics>::Unpack(wpi::ProtoInputStream& stream) {
  wpi_proto_ProtobufDifferentialDriveKinematics msg;
  if (!stream.DecodeNoInit(msg)) {
    return {};
  }

  return frc::DifferentialDriveKinematics{
      units::meter_t{msg.track_width},
  };
}

bool wpi::Protobuf<frc::DifferentialDriveKinematics>::Pack(
    wpi::ProtoOutputStream& stream,
    const frc::DifferentialDriveKinematics& value) {
  wpi_proto_ProtobufDifferentialDriveKinematics msg{
      .track_width = value.trackWidth.value(),
  };
  return stream.Encode(msg);
}
