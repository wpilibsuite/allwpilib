// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/proto/MecanumDriveWheelAccelerationsProto.hpp"

#include "wpimath/protobuf/kinematics.npb.h"

std::optional<wpi::math::MecanumDriveWheelAccelerations> wpi::util::Protobuf<
    wpi::math::MecanumDriveWheelAccelerations>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufMecanumDriveWheelAccelerations msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::MecanumDriveWheelAccelerations{
      units::meters_per_second_squared_t{msg.front_left},
      units::meters_per_second_squared_t{msg.front_right},
      units::meters_per_second_squared_t{msg.rear_left},
      units::meters_per_second_squared_t{msg.rear_right},
  };
}

bool wpi::util::Protobuf<wpi::math::MecanumDriveWheelAccelerations>::Pack(
    OutputStream& stream,
    const wpi::math::MecanumDriveWheelAccelerations& value) {
  wpi_proto_ProtobufMecanumDriveWheelAccelerations msg{
      .front_left = value.frontLeft.value(),
      .front_right = value.frontRight.value(),
      .rear_left = value.rearLeft.value(),
      .rear_right = value.rearRight.value(),
  };
  return stream.Encode(msg);
}
