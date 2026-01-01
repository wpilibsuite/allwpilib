// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/proto/ChassisAccelerationsProto.hpp"

#include "wpimath/protobuf/protobuf_kinematics.npb.h"

std::optional<wpi::math::ChassisAccelerations> wpi::util::Protobuf<
    wpi::math::ChassisAccelerations>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufChassisAccelerations msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::ChassisAccelerations{
      units::meters_per_second_squared_t{msg.ax},
      units::meters_per_second_squared_t{msg.ay},
      units::radians_per_second_squared_t{msg.alpha},
  };
}

bool wpi::util::Protobuf<wpi::math::ChassisAccelerations>::Pack(
    OutputStream& stream, const wpi::math::ChassisAccelerations& value) {
  wpi_proto_ProtobufChassisAccelerations msg{
      .ax = value.ax.value(),
      .ay = value.ay.value(),
      .alpha = value.alpha.value(),
  };
  return stream.Encode(msg);
}
