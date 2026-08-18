// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/proto/SwerveModulePositionProto.hpp"

#include "wpi/util/protobuf/ProtobufCallbacks.hpp"
#include "wpimath/protobuf/kinematics.npb.h"

std::optional<wpi::math::SwerveModulePosition> wpi::util::Protobuf<
    wpi::math::SwerveModulePosition>::Unpack(InputStream& stream) {
  wpi::util::UnpackCallback<wpi::math::Rotation2d> angle;
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
      wpi::units::meter_t{msg.distance},
      iangle[0],
  };
}

bool wpi::util::Protobuf<wpi::math::SwerveModulePosition>::Pack(
    OutputStream& stream, const wpi::math::SwerveModulePosition& value) {
  wpi::util::PackCallback angle{&value.angle};
  wpi_proto_ProtobufSwerveModulePosition msg{
      .distance = value.distance.value(),
      .angle = angle.Callback(),
  };
  return stream.Encode(msg);
}
