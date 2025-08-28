// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/protobuf/Protobuf.h>

#include "wpimath/controller/SimpleMotorFeedforward.h"
#include "pb.h"
#include "units/length.h"
#include "wpimath/protobuf/controller.npb.h"

// Everything is converted into units for
// wpimath::SimpleMotorFeedforward<units::meters> or
// wpimath::SimpleMotorFeedforward<units::radians>

template <class Distance>
  requires units::length_unit<Distance> || units::angle_unit<Distance>
struct wpi::Protobuf<wpimath::SimpleMotorFeedforward<Distance>> {
  using MessageStruct = wpi_proto_ProtobufSimpleMotorFeedforward;
  using InputStream =
      wpi::ProtoInputStream<wpimath::SimpleMotorFeedforward<Distance>>;
  using OutputStream =
      wpi::ProtoOutputStream<wpimath::SimpleMotorFeedforward<Distance>>;

  static std::optional<wpimath::SimpleMotorFeedforward<Distance>> Unpack(
      InputStream& stream) {
    using BaseUnit =
        units::unit<std::ratio<1>, units::traits::base_unit_of<Distance>>;
    using BaseFeedforward = wpimath::SimpleMotorFeedforward<BaseUnit>;
    wpi_proto_ProtobufSimpleMotorFeedforward msg;
    if (!stream.Decode(msg)) {
      return {};
    }

    return wpimath::SimpleMotorFeedforward<Distance>{
        units::volt_t{msg.ks},
        units::unit_t<typename BaseFeedforward::kv_unit>{msg.kv},
        units::unit_t<typename BaseFeedforward::ka_unit>{msg.ka},
        units::second_t{msg.dt},
    };
  }

  static bool Pack(OutputStream& stream,
                   const wpimath::SimpleMotorFeedforward<Distance>& value) {
    using BaseUnit =
        units::unit<std::ratio<1>, units::traits::base_unit_of<Distance>>;
    using BaseFeedforward = wpimath::SimpleMotorFeedforward<BaseUnit>;
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
