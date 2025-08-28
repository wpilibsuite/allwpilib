// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <optional>

#include <wpi/math/controller/proto/ArmFeedforwardProto.h>

#include "wpimath/protobuf/controller.npb.h"

std::optional<wpi::math::ArmFeedforward>
wpi::Protobuf<wpi::math::ArmFeedforward>::Unpack(InputStream& stream) {
  wpi_proto_ProtobufArmFeedforward msg;
  if (!stream.Decode(msg)) {
    return {};
  }

  return wpi::math::ArmFeedforward{
      units::volt_t{msg.ks},
      units::volt_t{msg.kg},
      units::unit_t<wpi::math::ArmFeedforward::kv_unit>{msg.kv},
      units::unit_t<wpi::math::ArmFeedforward::ka_unit>{msg.ka},
  };
}

bool wpi::Protobuf<wpi::math::ArmFeedforward>::Pack(
    OutputStream& stream, const wpi::math::ArmFeedforward& value) {
  wpi_proto_ProtobufArmFeedforward msg{
      .ks = value.GetKs().value(),
      .kg = value.GetKg().value(),
      .kv = value.GetKv().value(),
      .ka = value.GetKa().value(),
      .dt = 0,
  };
  return stream.Encode(msg);
}
