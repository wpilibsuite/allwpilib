// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/proto/ChassisVelocitiesProto.hpp"

#include "wpimath/protobuf/kinematics.npb.h"

std::optional<wpi::math::ChassisVelocities>
wpi::util::Protobuf<wpi::math::ChassisVelocities>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufChassisVelocities msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::ChassisVelocities{
      wpi::units::meters_per_second_t{msg.vx},
      wpi::units::meters_per_second_t{msg.vy},
      wpi::units::radians_per_second_t{msg.omega},
  };
}

bool wpi::util::Protobuf<wpi::math::ChassisVelocities>::Pack(
    OutputStream& stream, const wpi::math::ChassisVelocities& value) {
  wpi_proto_ProtobufChassisVelocities msg{
      .vx = value.vx.value(),
      .vy = value.vy.value(),
      .omega = value.omega.value(),
  };
  return stream.Encode(msg);
}
