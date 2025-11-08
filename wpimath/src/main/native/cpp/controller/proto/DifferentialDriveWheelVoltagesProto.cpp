// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/proto/DifferentialDriveWheelVoltagesProto.hpp"

#include <optional>

#include "wpimath/protobuf/controller.npb.h"

std::optional<wpi::math::DifferentialDriveWheelVoltages> wpi::util::Protobuf<
    wpi::math::DifferentialDriveWheelVoltages>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufDifferentialDriveWheelVoltages msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::DifferentialDriveWheelVoltages{
      wpi::units::volt_t{msg.left},
      wpi::units::volt_t{msg.right},
  };
}

bool wpi::util::Protobuf<wpi::math::DifferentialDriveWheelVoltages>::Pack(
    OutputStream& stream, const wpi::math::DifferentialDriveWheelVoltages& value) {
  wpi_proto_ProtobufDifferentialDriveWheelVoltages msg{
      .left = value.left.value(),
      .right = value.right.value(),
  };
  return stream.Encode(msg);
}
