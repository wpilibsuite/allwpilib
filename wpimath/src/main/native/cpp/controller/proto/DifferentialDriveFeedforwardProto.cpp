// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/proto/DifferentialDriveFeedforwardProto.hpp"

#include "wpimath/protobuf/controller.npb.h"

std::optional<wpi::math::DifferentialDriveFeedforward> wpi::util::Protobuf<
    wpi::math::DifferentialDriveFeedforward>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufDifferentialDriveFeedforward msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::DifferentialDriveFeedforward{
      decltype(1_V / 1_mps){msg.kvLinear},
      decltype(1_V / 1_mps2){msg.kaLinear},
      decltype(1_V / 1_mps){msg.kvAngular},
      decltype(1_V / 1_mps2){msg.kaAngular},
  };
}

bool wpi::util::Protobuf<wpi::math::DifferentialDriveFeedforward>::Pack(
    OutputStream& stream,
    const wpi::math::DifferentialDriveFeedforward& value) {
  wpi_proto_ProtobufDifferentialDriveFeedforward msg{
      .kvLinear = value.kvLinear.value(),
      .kaLinear = value.kaLinear.value(),
      .kvAngular = value.kvAngular.value(),
      .kaAngular = value.kaAngular.value(),
  };
  return stream.Encode(msg);
}
