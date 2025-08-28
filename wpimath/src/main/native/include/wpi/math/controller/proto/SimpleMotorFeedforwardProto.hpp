// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/math/controller/SimpleMotorFeedforward.hpp>
#include <wpi/protobuf/Protobuf.h>

#include "pb.h"
#include "units/length.hpp"
#include "wpimath/protobuf/controller.npb.hpp"

// Everything is converted into units for
// wpi::math::SimpleMotorFeedforward<units::meters> or
// wpi::math::SimpleMotorFeedforward<units::radians>

template <class Distance>
  requires units::length_unit<Distance> || units::angle_unit<Distance>
struct wpi::Protobuf<wpi::math::SimpleMotorFeedforward<Distance>> {
  using MessageStruct = wpi_proto_ProtobufSimpleMotorFeedforward;
  using InputStream =
      wpi::ProtoInputStream<wpi::math::SimpleMotorFeedforward<Distance>>;
  using OutputStream =
      wpi::ProtoOutputStream<wpi::math::SimpleMotorFeedforward<Distance>>;

  static std::optional<wpi::math::SimpleMotorFeedforward<Distance>> Unpack(
      InputStream& stream) {
    using BaseUnit =
        units::unit<std::ratio<1>, units::traits::base_unit_of<Distance>>;
    using BaseFeedforward = wpi::math::SimpleMotorFeedforward<BaseUnit>;
    wpi_proto_ProtobufSimpleMotorFeedforward msg;
    if (!stream.Decode(msg)) {
      return {};
    }

    return wpi::math::SimpleMotorFeedforward<Distance>{
        units::volt_t{msg.ks},
        units::unit_t<typename BaseFeedforward::kv_unit>{msg.kv},
        units::unit_t<typename BaseFeedforward::ka_unit>{msg.ka},
        units::second_t{msg.dt},
    };
  }

  static bool Pack(OutputStream& stream,
                   const wpi::math::SimpleMotorFeedforward<Distance>& value) {
    using BaseUnit =
        units::unit<std::ratio<1>, units::traits::base_unit_of<Distance>>;
    using BaseFeedforward = wpi::math::SimpleMotorFeedforward<BaseUnit>;
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
