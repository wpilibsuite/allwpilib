// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/proto/DifferentialDriveWheelVoltagesProto.h"

#include <optional>

#include "wpimath/protobuf/controller.npb.h"

std::optional<frc::DifferentialDriveWheelVoltages> wpi::Protobuf<
    frc::DifferentialDriveWheelVoltages>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufDifferentialDriveWheelVoltages msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return frc::DifferentialDriveWheelVoltages{
      units::volt_t{msg.left},
      units::volt_t{msg.right},
  };
}

bool wpi::Protobuf<frc::DifferentialDriveWheelVoltages>::Pack(
    OutputStream& stream, const frc::DifferentialDriveWheelVoltages& value) {
  wpi_proto_ProtobufDifferentialDriveWheelVoltages msg{
      .left = value.left.value(),
      .right = value.right.value(),
  };
  return stream.Encode(msg);
}
