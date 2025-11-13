// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/proto/SwerveModuleAccelerationsProto.hpp"

#include "wpi/util/protobuf/ProtobufCallbacks.hpp"
#include "wpimath/protobuf/kinematics.npb.h"

std::optional<wpi::math::SwerveModuleAccelerations> wpi::util::Protobuf<
    wpi::math::SwerveModuleAccelerations>::Unpack(InputStream& stream) {
  wpi::util::UnpackCallback<wpi::math::Rotation2d> angle;
  wpi_proto_ProtobufSwerveModuleAccelerations msg{
      .acceleration = 0,
      .angle = angle.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  auto iangle = angle.Items();

  if (iangle.empty()) {
    return {};
  }

  return wpi::math::SwerveModuleAccelerations{
      units::meters_per_second_squared_t{msg.acceleration},
      iangle[0],
  };
}

bool wpi::util::Protobuf<wpi::math::SwerveModuleAccelerations>::Pack(
    OutputStream& stream, const wpi::math::SwerveModuleAccelerations& value) {
  wpi::util::PackCallback angle{&value.angle};
  wpi_proto_ProtobufSwerveModuleAccelerations msg{
      .acceleration = value.acceleration.value(),
      .angle = angle.Callback(),
  };
  return stream.Encode(msg);
}
