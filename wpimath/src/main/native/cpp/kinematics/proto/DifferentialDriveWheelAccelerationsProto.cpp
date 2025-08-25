// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/DifferentialDriveWheelAccelerationsProto.h"

#include "wpimath/protobuf/kinematics.npb.h"

std::optional<frc::DifferentialDriveWheelAccelerations> wpi::Protobuf<
    frc::DifferentialDriveWheelAccelerations>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufDifferentialDriveWheelAccelerations msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return frc::DifferentialDriveWheelAccelerations{
      units::meters_per_second_squared_t{msg.left},
      units::meters_per_second_squared_t{msg.right},
  };
}

bool wpi::Protobuf<frc::DifferentialDriveWheelAccelerations>::Pack(
    OutputStream& stream,
    const frc::DifferentialDriveWheelAccelerations& value) {
  wpi_proto_ProtobufDifferentialDriveWheelAccelerations msg{
      .left = value.left.value(),
      .right = value.right.value(),
  };
  return stream.Encode(msg);
}
