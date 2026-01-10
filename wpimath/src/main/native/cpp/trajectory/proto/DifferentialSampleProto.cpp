// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/proto/DifferentialSampleProto.hpp"

#include "wpi/math/geometry/proto/Pose2dProto.hpp"
#include "wpi/math/kinematics/proto/ChassisAccelerationsProto.hpp"
#include "wpi/math/kinematics/proto/ChassisSpeedsProto.hpp"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"
#include "wpimath/protobuf/trajectory.npb.h"

std::optional<wpi::math::DifferentialSample> wpi::util::Protobuf<
    wpi::math::DifferentialSample>::Unpack(InputStream& stream) {
  wpi::util::UnpackCallback<wpi::math::Pose2d> pose;
  wpi::util::UnpackCallback<wpi::math::ChassisSpeeds> velocity;
  wpi::util::UnpackCallback<wpi::math::ChassisAccelerations> acceleration;
  wpi_proto_ProtobufDifferentialSample msg{
      .timestamp = 0,
      .pose = pose.Callback(),
      .velocities = velocity.Callback(),
      .accelerations = acceleration.Callback(),
      .left_velocity = 0,
      .right_velocity = 0,
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

  return wpi::math::DifferentialSample{
      wpi::units::second_t{msg.timestamp},
      iPose[0],
      iVel[0],
      iAccel[0],
      wpi::units::meters_per_second_t{msg.left_velocity},
      wpi::units::meters_per_second_t{msg.right_velocity},
  };
}

bool wpi::util::Protobuf<wpi::math::DifferentialSample>::Pack(
    OutputStream& stream, const wpi::math::DifferentialSample& value) {
  wpi::util::PackCallback pose{&value.pose};
  wpi::util::PackCallback velocity{&value.velocity};
  wpi::util::PackCallback acceleration{&value.acceleration};
  wpi_proto_ProtobufDifferentialSample msg{
      .timestamp = value.timestamp.value(),
      .pose = pose.Callback(),
      .velocities = velocity.Callback(),
      .accelerations = acceleration.Callback(),
      .left_velocity = value.leftSpeed.value(),
      .right_velocity = value.rightSpeed.value(),
  };
  return stream.Encode(msg);
}
