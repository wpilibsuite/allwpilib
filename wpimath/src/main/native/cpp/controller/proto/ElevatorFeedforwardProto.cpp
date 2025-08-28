// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/proto/ElevatorFeedforwardProto.h"

#include <optional>

#include "wpimath/protobuf/controller.npb.h"

std::optional<wpi::math::ElevatorFeedforward>
wpi::Protobuf<wpi::math::ElevatorFeedforward>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufElevatorFeedforward msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::ElevatorFeedforward{
      units::volt_t{msg.ks},
      units::volt_t{msg.kg},
      units::unit_t<wpi::math::ElevatorFeedforward::kv_unit>{msg.kv},
      units::unit_t<wpi::math::ElevatorFeedforward::ka_unit>{msg.ka},
  };
}

bool wpi::Protobuf<wpi::math::ElevatorFeedforward>::Pack(
    OutputStream& stream, const wpi::math::ElevatorFeedforward& value) {
  wpi_proto_ProtobufElevatorFeedforward msg{
      .ks = value.GetKs().value(),
      .kg = value.GetKg().value(),
      .kv = value.GetKv().value(),
      .ka = value.GetKa().value(),
      .dt = 0,
  };
  return stream.Encode(msg);
}
