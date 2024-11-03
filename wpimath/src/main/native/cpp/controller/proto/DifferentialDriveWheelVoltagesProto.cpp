// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/proto/DifferentialDriveWheelVoltagesProto.h"

#include <optional>

#include "controller.npb.h"

const pb_msgdesc_t*
wpi::Protobuf<frc::DifferentialDriveWheelVoltages>::Message() {
  return get_wpi_proto_ProtobufDifferentialDriveWheelVoltages_msg();
}

std::optional<frc::DifferentialDriveWheelVoltages>
wpi::Protobuf<frc::DifferentialDriveWheelVoltages>::Unpack(
    wpi::ProtoInputStream& stream) {
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
    wpi::ProtoOutputStream& stream,
    const frc::DifferentialDriveWheelVoltages& value) {
  wpi_proto_ProtobufDifferentialDriveWheelVoltages msg{
      .left = value.left.value(),
      .right = value.right.value(),
  };
  return stream.Encode(msg);
}
