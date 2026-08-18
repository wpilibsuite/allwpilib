// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/proto/HolonomicSampleProto.hpp"

#include "wpi/math/geometry/proto/Pose2dProto.hpp"
#include "wpi/math/kinematics/proto/ChassisAccelerationsProto.hpp"
#include "wpi/math/kinematics/proto/ChassisVelocitiesProto.hpp"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"
#include "wpimath/protobuf/trajectory.npb.h"

std::optional<wpi::math::HolonomicSample>
wpi::util::Protobuf<wpi::math::HolonomicSample>::Unpack(InputStream& stream) {
  wpi::util::UnpackCallback<wpi::math::Pose2d> pose;
  wpi::util::UnpackCallback<wpi::math::ChassisVelocities> velocity;
  wpi::util::UnpackCallback<wpi::math::ChassisAccelerations> acceleration;
  wpi_proto_ProtobufHolonomicSample msg{
      .time = 0,
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

  return wpi::math::HolonomicSample{
      wpi::units::second_t{msg.time},
      iPose[0],
      iVel[0],
      iAccel[0],
  };
}

bool wpi::util::Protobuf<wpi::math::HolonomicSample>::Pack(
    OutputStream& stream, const wpi::math::HolonomicSample& value) {
  wpi::util::PackCallback pose{&value.pose};
  wpi::util::PackCallback velocity{&value.velocity};
  wpi::util::PackCallback acceleration{&value.acceleration};
  wpi_proto_ProtobufHolonomicSample msg{
      .time = value.time.value(),
      .pose = pose.Callback(),
      .velocities = velocity.Callback(),
      .accelerations = acceleration.Callback(),
  };
  return stream.Encode(msg);
}
