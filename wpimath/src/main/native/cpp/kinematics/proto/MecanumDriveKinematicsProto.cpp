// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/proto/MecanumDriveKinematicsProto.hpp"

#include "wpi/util/protobuf/ProtobufCallbacks.hpp"
#include "wpimath/protobuf/protobuf_kinematics.npb.h"

std::optional<wpi::math::MecanumDriveKinematics> wpi::util::Protobuf<
    wpi::math::MecanumDriveKinematics>::Unpack(InputStream& stream) {
  wpi::util::UnpackCallback<wpi::math::Translation2d> frontLeft;
  wpi::util::UnpackCallback<wpi::math::Translation2d> frontRight;
  wpi::util::UnpackCallback<wpi::math::Translation2d> rearLeft;
  wpi::util::UnpackCallback<wpi::math::Translation2d> rearRight;
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

  return wpi::math::MecanumDriveKinematics{
      ifrontLeft[0],
      ifrontRight[0],
      irearLeft[0],
      irearRight[0],
  };
}

bool wpi::util::Protobuf<wpi::math::MecanumDriveKinematics>::Pack(
    OutputStream& stream, const wpi::math::MecanumDriveKinematics& value) {
  wpi::util::PackCallback frontLeft{&value.GetFrontLeft()};
  wpi::util::PackCallback frontRight{&value.GetFrontRight()};
  wpi::util::PackCallback rearLeft{&value.GetRearLeft()};
  wpi::util::PackCallback rearRight{&value.GetRearRight()};
  wpi_proto_ProtobufMecanumDriveKinematics msg{
      .front_left = frontLeft.Callback(),
      .front_right = frontRight.Callback(),
      .rear_left = rearLeft.Callback(),
      .rear_right = rearRight.Callback(),
  };
  return stream.Encode(msg);
}
