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
      decltype(1_V / 1_mps){msg.kv_linear},
      decltype(1_V / 1_mps_sq){msg.ka_linear},
      decltype(1_V / 1_mps){msg.kv_angular},
      decltype(1_V / 1_mps_sq){msg.ka_angular},
  };
}

bool wpi::util::Protobuf<wpi::math::DifferentialDriveFeedforward>::Pack(
    OutputStream& stream,
    const wpi::math::DifferentialDriveFeedforward& value) {
  wpi_proto_ProtobufDifferentialDriveFeedforward msg{
      .kv_linear = value.V_LINEAR.value(),
      .ka_linear = value.A_LINEAR.value(),
      .kv_angular = value.V_ANGULAR.value(),
      .ka_angular = value.A_ANGULAR.value(),
  };
  return stream.Encode(msg);
}
