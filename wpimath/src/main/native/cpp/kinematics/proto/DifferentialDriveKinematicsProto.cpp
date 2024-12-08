// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/DifferentialDriveKinematicsProto.h"

#include "wpimath/protobuf/kinematics.npb.h"

std::optional<frc::DifferentialDriveKinematics>
wpi::Protobuf<frc::DifferentialDriveKinematics>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufDifferentialDriveKinematics msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return frc::DifferentialDriveKinematics{
      units::meter_t{msg.trackwidth},
  };
}

bool wpi::Protobuf<frc::DifferentialDriveKinematics>::Pack(
    OutputStream& stream, const frc::DifferentialDriveKinematics& value) {
  wpi_proto_ProtobufDifferentialDriveKinematics msg{
      .trackwidth = value.trackwidth.value(),
  };
  return stream.Encode(msg);
}
