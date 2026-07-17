// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/proto/TrajectorySampleProto.hpp"

#include "wpimath/protobuf/trajectory.npb.h"

std::optional<wpi::math::TrajectorySample>
wpi::util::Protobuf<wpi::math::TrajectorySample>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufTrajectorySample msg{.time = 0};
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::TrajectorySample{wpi::units::second_t{msg.time}};
}

bool wpi::util::Protobuf<wpi::math::TrajectorySample>::Pack(
    OutputStream& stream, const wpi::math::TrajectorySample& value) {
  wpi_proto_ProtobufTrajectorySample msg{.time = value.time.value()};
  return stream.Encode(msg);
}
