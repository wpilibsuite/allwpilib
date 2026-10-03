// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/proto/DifferentialTrajectoryProto.hpp"

#include <utility>

#include "wpi/math/trajectory/proto/DifferentialSampleProto.hpp"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"
#include "wpimath/protobuf/trajectory.npb.h"

std::optional<wpi::math::DifferentialTrajectory> wpi::util::Protobuf<
    wpi::math::DifferentialTrajectory>::Unpack(InputStream& stream) {
  wpi::util::StdVectorUnpackCallback<wpi::math::DifferentialSample, SIZE_MAX>
      samples;
  wpi_proto_ProtobufDifferentialTrajectory msg{
      .samples = samples.Callback(),
  };
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::DifferentialTrajectory{std::move(samples.Vec())};
}

bool wpi::util::Protobuf<wpi::math::DifferentialTrajectory>::Pack(
    OutputStream& stream, const wpi::math::DifferentialTrajectory& value) {
  wpi::util::PackCallback<wpi::math::DifferentialSample> samples{
      value.Samples()};
  wpi_proto_ProtobufDifferentialTrajectory msg{
      .samples = samples.Callback(),
  };
  return stream.Encode(msg);
}
