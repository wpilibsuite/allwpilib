// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/kinematics/proto/DifferentialDriveWheelSpeedsProto.h"

#include "wpimath/protobuf/kinematics.npb.h"

std::optional<wpimath::DifferentialDriveWheelSpeeds> wpi::Protobuf<
    wpimath::DifferentialDriveWheelSpeeds>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufDifferentialDriveWheelSpeeds msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpimath::DifferentialDriveWheelSpeeds{
      units::meters_per_second_t{msg.left},
      units::meters_per_second_t{msg.right},
  };
}

bool wpi::Protobuf<wpimath::DifferentialDriveWheelSpeeds>::Pack(
    OutputStream& stream, const wpimath::DifferentialDriveWheelSpeeds& value) {
  wpi_proto_ProtobufDifferentialDriveWheelSpeeds msg{
      .left = value.left.value(),
      .right = value.right.value(),
  };
  return stream.Encode(msg);
}
