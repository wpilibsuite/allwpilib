// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/protobuf/Protobuf.h>

#include "frc/controller/ArmFeedforward.h"
#include "pb.h"
#include "wpimath/protobuf/controller.npb.h"

template <class Input>
  requires(units::current_unit<Input> || units::voltage_unit<Input>)
struct wpi::Protobuf<frc::ArmFeedforward<Input>> {
  using MessageStruct = wpi_proto_ProtobufArmFeedforward;
  using InputStream = wpi::ProtoInputStream<frc::ArmFeedforward<Input>>;
  using OutputStream = wpi::ProtoOutputStream<frc::ArmFeedforward<Input>>;

  static std::optional<frc::ArmFeedforward<Input>> Unpack(InputStream& stream) {
    using InputUnit =
        units::unit<std::ratio<1>, units::traits::base_unit_of<Input>>;
    using BaseFeedforward = frc::ArmFeedforward<InputUnit>;
    wpi_proto_ProtobufArmFeedforward msg;
    if (!stream.Decode(msg)) {
      return {};
    }

    return frc::ArmFeedforward<Input>{
        units::unit_t<Input>{msg.ks},
        units::unit_t<Input>{msg.kg},
        units::unit_t<typename BaseFeedforward::kv_unit>{msg.kv},
        units::unit_t<typename BaseFeedforward::ka_unit>{msg.ka},
        units::second_t{msg.dt},
    };
  }

  static bool Pack(OutputStream& stream,
                   const frc::ArmFeedforward<Input>& value) {
    using InputUnit =
        units::unit<std::ratio<1>, units::traits::base_unit_of<Input>>;
    using BaseFeedforward = frc::ArmFeedforward<InputUnit>;
    wpi_proto_ProtobufArmFeedforward msg{
        .ks = value.GetKs().value(),
        .kg = value.GetKg().value(),
        .kv = units::unit_t<typename BaseFeedforward::kv_unit>{value.GetKv()}
                  .value(),
        .ka = units::unit_t<typename BaseFeedforward::ka_unit>{value.GetKa()}
                  .value(),
        .dt = units::second_t{value.GetDt()}.value(),
    };
    return stream.Encode(msg);
  }
};
