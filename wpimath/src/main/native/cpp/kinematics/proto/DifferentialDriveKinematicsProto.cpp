// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/proto/DifferentialDriveKinematicsProto.hpp"

#include "wpimath/protobuf/kinematics.npb.h"

std::optional<wpi::math::DifferentialDriveKinematics> wpi::util::Protobuf<
    wpi::math::DifferentialDriveKinematics>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufDifferentialDriveKinematics msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::DifferentialDriveKinematics{
      wpi::units::meter_t{msg.trackwidth},
  };
}

bool wpi::util::Protobuf<wpi::math::DifferentialDriveKinematics>::Pack(
    OutputStream& stream, const wpi::math::DifferentialDriveKinematics& value) {
  wpi_proto_ProtobufDifferentialDriveKinematics msg{
      .trackwidth = value.trackwidth.value(),
  };
  return stream.Encode(msg);
}
