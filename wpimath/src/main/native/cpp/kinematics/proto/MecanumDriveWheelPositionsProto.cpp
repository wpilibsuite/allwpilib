// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/proto/MecanumDriveWheelPositionsProto.hpp"

#include "wpimath/protobuf/kinematics.npb.h"

std::optional<wpi::math::MecanumDriveWheelPositions> wpi::util::Protobuf<
    wpi::math::MecanumDriveWheelPositions>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufMecanumDriveWheelPositions msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::MecanumDriveWheelPositions{
      wpi::units::meter_t{msg.front_left},
      wpi::units::meter_t{msg.front_right},
      wpi::units::meter_t{msg.rear_left},
      wpi::units::meter_t{msg.rear_right},
  };
}

bool wpi::util::Protobuf<wpi::math::MecanumDriveWheelPositions>::Pack(
    OutputStream& stream, const wpi::math::MecanumDriveWheelPositions& value) {
  wpi_proto_ProtobufMecanumDriveWheelPositions msg{
      .front_left = value.frontLeft.value(),
      .front_right = value.frontRight.value(),
      .rear_left = value.rearLeft.value(),
      .rear_right = value.rearRight.value(),
  };
  return stream.Encode(msg);
}
