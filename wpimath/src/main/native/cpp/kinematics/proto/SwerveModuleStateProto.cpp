// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/SwerveModuleStateProto.h"

#include "kinematics.npb.h"
#include "wpi/protobuf/ProtobufCallbacks.h"

const pb_msgdesc_t* wpi::Protobuf<frc::SwerveModuleState>::Message() {
  return get_wpi_proto_ProtobufSwerveModuleState_msg();
}

std::optional<frc::SwerveModuleState>
wpi::Protobuf<frc::SwerveModuleState>::Unpack(wpi::ProtoInputStream& stream) {
  wpi::UnpackCallback<frc::Rotation2d> angle;
  wpi_proto_ProtobufSwerveModuleState msg{
      .speed = 0,
      .angle = angle.Callback(),
  };
  if (!stream.DecodeNoInit(msg)) {
    return {};
  }

  auto iangle = angle.Items();

  if (iangle.empty()) {
    return {};
  }

  return frc::SwerveModuleState{
      units::meters_per_second_t{msg.speed},
      iangle[0],
  };
}

bool wpi::Protobuf<frc::SwerveModuleState>::Pack(
    wpi::ProtoOutputStream& stream, const frc::SwerveModuleState& value) {
  wpi::PackCallback angle{&value.angle};
  wpi_proto_ProtobufSwerveModuleState msg{
      .speed = value.speed.value(),
      .angle = angle.Callback(),
  };
  return stream.Encode(msg);
}
