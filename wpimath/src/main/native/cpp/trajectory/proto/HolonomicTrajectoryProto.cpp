// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/proto/HolonomicTrajectoryProto.hpp"

#include <utility>

#include "wpi/math/trajectory/proto/TrajectorySampleProto.hpp"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"
#include "wpimath/protobuf/trajectory.npb.h"

std::optional<wpi::math::HolonomicTrajectory> wpi::util::Protobuf<
    wpi::math::HolonomicTrajectory>::Unpack(InputStream& stream) {
  wpi::util::StdVectorUnpackCallback<wpi::math::TrajectorySample, SIZE_MAX>
      samples;
  wpi_proto_ProtobufHolonomicTrajectory msg{
      .samples = samples.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::HolonomicTrajectory{std::move(samples.Vec())};
}

bool wpi::util::Protobuf<wpi::math::HolonomicTrajectory>::Pack(
    OutputStream& stream, const wpi::math::HolonomicTrajectory& value) {
  wpi::util::PackCallback<wpi::math::TrajectorySample> samples{value.Samples()};
  wpi_proto_ProtobufHolonomicTrajectory msg{
      .samples = samples.Callback(),
  };
  return stream.Encode(msg);
}
