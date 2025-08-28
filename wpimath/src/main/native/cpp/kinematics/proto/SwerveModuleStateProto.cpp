// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/proto/SwerveModuleStateProto.h"

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "wpimath/protobuf/kinematics.npb.h"

std::optional<wpi::math::SwerveModuleState>
wpi::Protobuf<wpi::math::SwerveModuleState>::Unpack(InputStream& stream) {
  wpi::UnpackCallback<wpi::math::Rotation2d> angle;
  wpi_proto_ProtobufSwerveModuleState msg{
      .speed = 0,
      .angle = angle.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  auto iangle = angle.Items();

  if (iangle.empty()) {
    return {};
  }

  return wpi::math::SwerveModuleState{
      units::meters_per_second_t{msg.speed},
      iangle[0],
  };
}

bool wpi::Protobuf<wpi::math::SwerveModuleState>::Pack(
    OutputStream& stream, const wpi::math::SwerveModuleState& value) {
  wpi::PackCallback angle{&value.angle};
  wpi_proto_ProtobufSwerveModuleState msg{
      .speed = value.speed.value(),
      .angle = angle.Callback(),
  };
  return stream.Encode(msg);
}
