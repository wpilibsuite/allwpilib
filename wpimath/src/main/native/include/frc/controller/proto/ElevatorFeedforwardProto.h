// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/protobuf/Protobuf.h>

#include "frc/controller/ElevatorFeedforward.h"
#include "pb.h"
#include "units/length.h"
#include "wpimath/protobuf/controller.npb.h"

template <class Input>
  requires(units::current_unit<Input> || units::voltage_unit<Input>)
struct wpi::Protobuf<frc::ElevatorFeedforward<Input>> {
  using MessageStruct = wpi_proto_ProtobufElevatorFeedforward;
  using InputStream = wpi::ProtoInputStream<frc::ElevatorFeedforward<Input>>;
  using OutputStream = wpi::ProtoOutputStream<frc::ElevatorFeedforward<Input>>;

  static std::optional<frc::ElevatorFeedforward<Input>> Unpack(
      InputStream& stream) {
    using InputUnit =
        units::unit<std::ratio<1>, units::traits::base_unit_of<Input>>;
    using BaseFeedforward = frc::ElevatorFeedforward<InputUnit>;
    wpi_proto_ProtobufElevatorFeedforward msg;
    if (!stream.Decode(msg)) {
      return {};
    }

    return frc::ElevatorFeedforward<Input>{
        units::unit_t<Input>{msg.ks},
        units::unit_t<Input>{msg.kg},
        units::unit_t<typename BaseFeedforward::kv_unit>{msg.kv},
        units::unit_t<typename BaseFeedforward::ka_unit>{msg.ka},
        units::second_t{msg.dt},
    };
  }

  static bool Pack(OutputStream& stream,
                   const frc::ElevatorFeedforward<Input>& value) {
    using InputUnit =
        units::unit<std::ratio<1>, units::traits::base_unit_of<Input>>;
    using BaseFeedforward = frc::ElevatorFeedforward<InputUnit>;
    wpi_proto_ProtobufElevatorFeedforward msg{
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
