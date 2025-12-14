// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/proto/SwerveModuleVelocityProto.hpp"

#include "wpi/util/protobuf/ProtobufCallbacks.hpp"
#include "wpimath/protobuf/kinematics.npb.h"

std::optional<wpi::math::SwerveModuleVelocity> wpi::util::Protobuf<
    wpi::math::SwerveModuleVelocity>::Unpack(InputStream& stream) {
  wpi::util::UnpackCallback<wpi::math::Rotation2d> angle;
  wpi_proto_ProtobufSwerveModuleVelocity msg{
      .velocity = 0,
      .angle = angle.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  auto iangle = angle.Items();

  if (iangle.empty()) {
    return {};
  }

  return wpi::math::SwerveModuleVelocity{
      wpi::units::meters_per_second_t{msg.velocity},
      iangle[0],
  };
}

bool wpi::util::Protobuf<wpi::math::SwerveModuleVelocity>::Pack(
    OutputStream& stream, const wpi::math::SwerveModuleVelocity& value) {
  wpi::util::PackCallback angle{&value.angle};
  wpi_proto_ProtobufSwerveModuleVelocity msg{
      .velocity = value.velocity.value(),
      .angle = angle.Callback(),
  };
  return stream.Encode(msg);
}
