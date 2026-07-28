// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/proto/MecanumDriveWheelVelocitiesProto.hpp"

#include "wpimath/protobuf/kinematics.npb.h"

std::optional<wpi::math::MecanumDriveWheelVelocities> wpi::util::Protobuf<
    wpi::math::MecanumDriveWheelVelocities>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufMecanumDriveWheelVelocities msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::MecanumDriveWheelVelocities{
      wpi::units::meters_per_second_t{msg.front_left},
      wpi::units::meters_per_second_t{msg.front_right},
      wpi::units::meters_per_second_t{msg.rear_left},
      wpi::units::meters_per_second_t{msg.rear_right},
  };
}

bool wpi::util::Protobuf<wpi::math::MecanumDriveWheelVelocities>::Pack(
    OutputStream& stream, const wpi::math::MecanumDriveWheelVelocities& value) {
  wpi_proto_ProtobufMecanumDriveWheelVelocities msg{
      .front_left = value.frontLeft.value(),
      .front_right = value.frontRight.value(),
      .rear_left = value.rearLeft.value(),
      .rear_right = value.rearRight.value(),
  };
  return stream.Encode(msg);
}
