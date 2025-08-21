// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/SwerveModuleAccelerationsProto.h"

#include "wpimath/protobuf/kinematics.npb.h"

std::optional<frc::SwerveModuleAccelerations> wpi::Protobuf<frc::SwerveModuleAccelerations>::Unpack(
    InputStream& stream) {
  wpi_proto_ProtobufSwerveModuleAccelerations msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return frc::SwerveModuleAccelerations{
      units::meters_per_second_squared_t{msg.acceleration},
      units::radians_per_second_squared_t{msg.angular_acceleration},
  };
}

bool wpi::Protobuf<frc::SwerveModuleAccelerations>::Pack(OutputStream& stream,
                                                        const frc::SwerveModuleAccelerations& value) {
  wpi_proto_ProtobufSwerveModuleAccelerations msg{
      .acceleration = value.acceleration.value(),
      .angular_acceleration = value.angularAcceleration.value(),
  };
  return stream.Encode(msg);
}
