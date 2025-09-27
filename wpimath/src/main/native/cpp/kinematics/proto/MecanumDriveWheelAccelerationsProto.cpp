// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/MecanumDriveWheelAccelerationsProto.h"

#include "wpimath/protobuf/kinematics.npb.h"

std::optional<frc::MecanumDriveWheelAccelerations> wpi::Protobuf<
    frc::MecanumDriveWheelAccelerations>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufMecanumDriveWheelAccelerations msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return frc::MecanumDriveWheelAccelerations{
      units::meters_per_second_squared_t{msg.front_left},
      units::meters_per_second_squared_t{msg.front_right},
      units::meters_per_second_squared_t{msg.rear_left},
      units::meters_per_second_squared_t{msg.rear_right},
  };
}

bool wpi::Protobuf<frc::MecanumDriveWheelAccelerations>::Pack(
    OutputStream& stream, const frc::MecanumDriveWheelAccelerations& value) {
  wpi_proto_ProtobufMecanumDriveWheelAccelerations msg{
      .front_left = value.frontLeft.value(),
      .front_right = value.frontRight.value(),
      .rear_left = value.rearLeft.value(),
      .rear_right = value.rearRight.value(),
  };
  return stream.Encode(msg);
}
