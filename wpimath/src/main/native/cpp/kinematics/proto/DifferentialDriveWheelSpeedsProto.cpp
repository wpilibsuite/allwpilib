// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/proto/DifferentialDriveWheelSpeedsProto.hpp"

#include "wpimath/protobuf/kinematics.npb.h"

std::optional<wpi::math::DifferentialDriveWheelSpeeds>
wpi::util::Protobuf<wpi::math::DifferentialDriveWheelSpeeds>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufDifferentialDriveWheelSpeeds msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::DifferentialDriveWheelSpeeds{
      wpi::units::meters_per_second_t{msg.left},
      wpi::units::meters_per_second_t{msg.right},
  };
}

bool wpi::util::Protobuf<wpi::math::DifferentialDriveWheelSpeeds>::Pack(
    OutputStream& stream, const wpi::math::DifferentialDriveWheelSpeeds& value) {
  wpi_proto_ProtobufDifferentialDriveWheelSpeeds msg{
      .left = value.left.value(),
      .right = value.right.value(),
  };
  return stream.Encode(msg);
}
