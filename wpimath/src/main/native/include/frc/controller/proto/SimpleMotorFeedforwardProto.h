// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/protobuf/Protobuf.h>

#include "frc/controller/SimpleMotorFeedforward.h"
#include "pb.h"
#include "units/length.h"
#include "wpimath/protobuf/controller.npb.h"

template <class Distance, class Input>
  requires(units::length_unit<Distance> || units::angle_unit<Distance>) &&
          (units::current_unit<Input> || units::voltage_unit<Input>)
struct wpi::Protobuf<frc::SimpleMotorFeedforward<Distance, Input>> {
  using MessageStruct = wpi_proto_ProtobufSimpleMotorFeedforward;
  using InputStream =
      wpi::ProtoInputStream<frc::SimpleMotorFeedforward<Distance, Input>>;
  using OutputStream =
      wpi::ProtoOutputStream<frc::SimpleMotorFeedforward<Distance, Input>>;

  static std::optional<frc::SimpleMotorFeedforward<Distance, Input>> Unpack(
      InputStream& stream) {
    using BaseUnit =
        units::unit<std::ratio<1>, units::traits::base_unit_of<Distance>>;
    using InputUnit =
        units::unit<std::ratio<1>, units::traits::base_unit_of<Input>>;
    using BaseFeedforward = frc::SimpleMotorFeedforward<BaseUnit, InputUnit>;
    wpi_proto_ProtobufSimpleMotorFeedforward msg;
    if (!stream.Decode(msg)) {
      return {};
    }

    return frc::SimpleMotorFeedforward<Distance, Input>{
        units::unit_t<Input>{msg.ks},
        units::unit_t<typename BaseFeedforward::kv_unit>{msg.kv},
        units::unit_t<typename BaseFeedforward::ka_unit>{msg.ka},
        units::second_t{msg.dt},
    };
  }

  static bool Pack(OutputStream& stream,
                   const frc::SimpleMotorFeedforward<Distance, Input>& value) {
    using BaseUnit =
        units::unit<std::ratio<1>, units::traits::base_unit_of<Distance>>;
    using InputUnit =
        units::unit<std::ratio<1>, units::traits::base_unit_of<Input>>;
    using BaseFeedforward = frc::SimpleMotorFeedforward<BaseUnit, InputUnit>;
    wpi_proto_ProtobufSimpleMotorFeedforward msg{
        .ks = value.GetKs().value(),
        .kv = units::unit_t<typename BaseFeedforward::kv_unit>{value.GetKv()}
                  .value(),
        .ka = units::unit_t<typename BaseFeedforward::ka_unit>{value.GetKa()}
                  .value(),
        .dt = units::second_t{value.GetDt()}.value(),
    };
    return stream.Encode(msg);
  }
};
