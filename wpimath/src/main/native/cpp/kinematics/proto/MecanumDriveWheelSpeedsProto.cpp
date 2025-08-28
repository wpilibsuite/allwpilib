// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/kinematics/proto/MecanumDriveWheelSpeedsProto.h"

#include "wpimath/protobuf/kinematics.npb.h"

std::optional<wpimath::MecanumDriveWheelSpeeds>
wpi::Protobuf<wpimath::MecanumDriveWheelSpeeds>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufMecanumDriveWheelSpeeds msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpimath::MecanumDriveWheelSpeeds{
      units::meters_per_second_t{msg.front_left},
      units::meters_per_second_t{msg.front_right},
      units::meters_per_second_t{msg.rear_left},
      units::meters_per_second_t{msg.rear_right},
  };
}

bool wpi::Protobuf<wpimath::MecanumDriveWheelSpeeds>::Pack(
    OutputStream& stream, const wpimath::MecanumDriveWheelSpeeds& value) {
  wpi_proto_ProtobufMecanumDriveWheelSpeeds msg{
      .front_left = value.frontLeft.value(),
      .front_right = value.frontRight.value(),
      .rear_left = value.rearLeft.value(),
      .rear_right = value.rearRight.value(),
  };
  return stream.Encode(msg);
}
