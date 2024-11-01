// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/ChassisSpeedsProto.h"

#include "kinematics.npb.h"

const pb_msgdesc_t* wpi::Protobuf<frc::ChassisSpeeds>::Message() {
  return get_wpi_proto_ProtobufChassisSpeeds_msg();
}

std::optional<frc::ChassisSpeeds> wpi::Protobuf<frc::ChassisSpeeds>::Unpack(
    wpi::ProtoInputStream& stream) {
  wpi_proto_ProtobufChassisSpeeds msg;
  if (!stream.DecodeNoInit(msg)) {
    return {};
  }

  return frc::ChassisSpeeds{
      units::meters_per_second_t{msg.vx},
      units::meters_per_second_t{msg.vy},
      units::radians_per_second_t{msg.omega},
  };
}

bool wpi::Protobuf<frc::ChassisSpeeds>::Pack(wpi::ProtoOutputStream& stream,
                                             const frc::ChassisSpeeds& value) {
  wpi_proto_ProtobufChassisSpeeds msg{
      .vx = value.vx.value(),
      .vy = value.vy.value(),
      .omega = value.omega.value(),
  };
  return stream.Encode(msg);
}
