// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/SwerveModuleAccelerationsProto.h"

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "wpimath/protobuf/kinematics.npb.h"

std::optional<frc::SwerveModuleAccelerations>
wpi::Protobuf<frc::SwerveModuleAccelerations>::Unpack(InputStream& stream) {
  wpi::UnpackCallback<frc::Rotation2d> angle;
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

  return frc::SwerveModuleAccelerations{
      units::meters_per_second_squared_t{msg.acceleration},
      iangle[0],
  };
}

bool wpi::Protobuf<frc::SwerveModuleAccelerations>::Pack(
    OutputStream& stream, const frc::SwerveModuleAccelerations& value) {
  wpi::PackCallback angle{&value.angle};
  wpi_proto_ProtobufSwerveModuleAccelerations msg{
      .acceleration = value.acceleration.value(),
      .angle = angle.Callback(),
  };
  return stream.Encode(msg);
}
