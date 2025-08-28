// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/math/kinematics/proto/SwerveModulePositionProto.h>
#include <wpi/protobuf/ProtobufCallbacks.h>

#include "wpimath/protobuf/kinematics.npb.h"

std::optional<wpi::math::SwerveModulePosition>
wpi::Protobuf<wpi::math::SwerveModulePosition>::Unpack(InputStream& stream) {
  wpi::UnpackCallback<wpi::math::Rotation2d> angle;
  wpi_proto_ProtobufSwerveModulePosition msg{
      .distance = 0,
      .angle = angle.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  auto iangle = angle.Items();

  if (iangle.empty()) {
    return {};
  }

  return wpi::math::SwerveModulePosition{
      units::meter_t{msg.distance},
      iangle[0],
  };
}

bool wpi::Protobuf<wpi::math::SwerveModulePosition>::Pack(
    OutputStream& stream, const wpi::math::SwerveModulePosition& value) {
  wpi::PackCallback angle{&value.angle};
  wpi_proto_ProtobufSwerveModulePosition msg{
      .distance = value.distance.value(),
      .angle = angle.Callback(),
  };
  return stream.Encode(msg);
}
