// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/proto/TrajectorySampleProto.hpp"

#include "wpi/math/geometry/proto/Pose2dProto.hpp"
#include "wpi/math/kinematics/proto/ChassisAccelerationsProto.hpp"
#include "wpi/math/kinematics/proto/ChassisSpeedsProto.hpp"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"
#include "wpimath/protobuf/trajectory.npb.h"

std::optional<wpi::math::TrajectorySample>
wpi::util::Protobuf<wpi::math::TrajectorySample>::Unpack(InputStream& stream) {
  wpi::util::UnpackCallback<wpi::math::Pose2d> pose;
  wpi::util::UnpackCallback<wpi::math::ChassisSpeeds> velocity;
  wpi::util::UnpackCallback<wpi::math::ChassisAccelerations> acceleration;
  wpi_proto_ProtobufTrajectorySample msg{
      .timestamp = 0,
      .pose = pose.Callback(),
      .velocities = velocity.Callback(),
      .accelerations = acceleration.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  auto iPose = pose.Items();
  auto iVel = velocity.Items();
  auto iAccel = acceleration.Items();

  if (iPose.empty() || iVel.empty() || iAccel.empty()) {
    return {};
  }

  return wpi::math::TrajectorySample{
      wpi::units::second_t{msg.timestamp},
      iPose[0],
      iVel[0],
      iAccel[0],
  };
}

bool wpi::util::Protobuf<wpi::math::TrajectorySample>::Pack(
    OutputStream& stream, const wpi::math::TrajectorySample& value) {
  wpi::util::PackCallback pose{&value.pose};
  wpi::util::PackCallback velocity{&value.velocity};
  wpi::util::PackCallback acceleration{&value.acceleration};
  wpi_proto_ProtobufTrajectorySample msg{
      .timestamp = value.timestamp.value(),
      .pose = pose.Callback(),
      .velocities = velocity.Callback(),
      .accelerations = acceleration.Callback(),
  };
  return stream.Encode(msg);
}
