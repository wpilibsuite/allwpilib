// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/proto/ChassisSpeedsProto.h"

#include "wpimath/protobuf/kinematics.npb.h"

std::optional<wpi::math::ChassisSpeeds>
wpi::Protobuf<wpi::math::ChassisSpeeds>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufChassisSpeeds msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::ChassisSpeeds{
      units::meters_per_second_t{msg.vx},
      units::meters_per_second_t{msg.vy},
      units::radians_per_second_t{msg.omega},
  };
}

bool wpi::Protobuf<wpi::math::ChassisSpeeds>::Pack(
    OutputStream& stream, const wpi::math::ChassisSpeeds& value) {
  wpi_proto_ProtobufChassisSpeeds msg{
      .vx = value.vx.value(),
      .vy = value.vy.value(),
      .omega = value.omega.value(),
  };
  return stream.Encode(msg);
}
