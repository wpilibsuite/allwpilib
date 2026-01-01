// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/proto/DifferentialDriveWheelAccelerationsProto.hpp"

#include "wpimath/protobuf/protobuf_kinematics.npb.h"

std::optional<wpi::math::DifferentialDriveWheelAccelerations>
wpi::util::Protobuf<wpi::math::DifferentialDriveWheelAccelerations>::Unpack(
    InputStream& stream) {
  wpi_proto_ProtobufDifferentialDriveWheelAccelerations msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::DifferentialDriveWheelAccelerations{
      units::meters_per_second_squared_t{msg.left},
      units::meters_per_second_squared_t{msg.right},
  };
}

bool wpi::util::Protobuf<wpi::math::DifferentialDriveWheelAccelerations>::Pack(
    OutputStream& stream,
    const wpi::math::DifferentialDriveWheelAccelerations& value) {
  wpi_proto_ProtobufDifferentialDriveWheelAccelerations msg{
      .left = value.left.value(),
      .right = value.right.value(),
  };
  return stream.Encode(msg);
}
