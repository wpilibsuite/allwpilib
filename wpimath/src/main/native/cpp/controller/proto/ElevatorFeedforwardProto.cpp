// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/proto/ElevatorFeedforwardProto.hpp"

#include <optional>

#include "wpi/math/protobuf/controller.npb.h"

std::optional<wpi::math::ElevatorFeedforward> wpi::util::Protobuf<
    wpi::math::ElevatorFeedforward>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufElevatorFeedforward msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::ElevatorFeedforward{
      wpi::units::volt_t{msg.ks},
      wpi::units::volt_t{msg.kg},
      wpi::units::unit_t<wpi::math::ElevatorFeedforward::kv_unit>{msg.kv},
      wpi::units::unit_t<wpi::math::ElevatorFeedforward::ka_unit>{msg.ka},
      // default dt for back-compat with old versions
      wpi::units::second_t{msg.dt > 0 ? msg.dt : 0.020},
  };
}

bool wpi::util::Protobuf<wpi::math::ElevatorFeedforward>::Pack(
    OutputStream& stream, const wpi::math::ElevatorFeedforward& value) {
  wpi_proto_ProtobufElevatorFeedforward msg{
      .ks = value.GetKs().value(),
      .kg = value.GetKg().value(),
      .kv = value.GetKv().value(),
      .ka = value.GetKa().value(),
      .dt = value.GetDt().value(),
  };
  return stream.Encode(msg);
}
