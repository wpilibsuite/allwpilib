// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/proto/MecanumDriveKinematicsProto.h"

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "wpimath/protobuf/kinematics.npb.h"

std::optional<frc::MecanumDriveKinematics>
wpi::Protobuf<frc::MecanumDriveKinematics>::Unpack(InputStream& stream) {
  wpi::UnpackCallback<frc::Translation2d> frontLeft;
  wpi::UnpackCallback<frc::Translation2d> frontRight;
  wpi::UnpackCallback<frc::Translation2d> rearLeft;
  wpi::UnpackCallback<frc::Translation2d> rearRight;
  wpi_proto_ProtobufMecanumDriveKinematics msg{
      .front_left = frontLeft.Callback(),
      .front_right = frontRight.Callback(),
      .rear_left = rearLeft.Callback(),
      .rear_right = rearRight.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  auto ifrontLeft = frontLeft.Items();
  auto ifrontRight = frontRight.Items();
  auto irearLeft = rearLeft.Items();
  auto irearRight = rearRight.Items();

  if (ifrontLeft.empty() || ifrontRight.empty() || irearLeft.empty() ||
      irearRight.empty()) {
    return {};
  }

  return frc::MecanumDriveKinematics{
      ifrontLeft[0],
      ifrontRight[0],
      irearLeft[0],
      irearRight[0],
  };
}

bool wpi::Protobuf<frc::MecanumDriveKinematics>::Pack(
    OutputStream& stream, const frc::MecanumDriveKinematics& value) {
  wpi::PackCallback frontLeft{&value.GetFrontLeft()};
  wpi::PackCallback frontRight{&value.GetFrontRight()};
  wpi::PackCallback rearLeft{&value.GetRearLeft()};
  wpi::PackCallback rearRight{&value.GetRearRight()};
  wpi_proto_ProtobufMecanumDriveKinematics msg{
      .front_left = frontLeft.Callback(),
      .front_right = frontRight.Callback(),
      .rear_left = rearLeft.Callback(),
      .rear_right = rearRight.Callback(),
  };
  return stream.Encode(msg);
}
