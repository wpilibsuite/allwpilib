// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "pb.h"
#include "wpi/math/controller/SimpleMotorFeedforward.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/controller.npb.h"

// Everything is converted into units for
// wpi::math::SimpleMotorFeedforward<wpi::units::meters_> or
// wpi::math::SimpleMotorFeedforward<wpi::units::radians_>

template <class Distance>
  requires wpi::units::length_unit<Distance> ||
           wpi::units::angle_unit<Distance> ||
           wpi::units::dimensionless_unit<Distance>
struct wpi::util::Protobuf<wpi::math::SimpleMotorFeedforward<Distance>> {
  using MessageStruct = wpi_proto_ProtobufSimpleMotorFeedforward;
  using InputStream =
      wpi::util::ProtoInputStream<wpi::math::SimpleMotorFeedforward<Distance>>;
  using OutputStream =
      wpi::util::ProtoOutputStream<wpi::math::SimpleMotorFeedforward<Distance>>;

  static std::optional<wpi::math::SimpleMotorFeedforward<Distance>> Unpack(
      InputStream& stream) {
    using BaseUnit = wpi::units::conversion_factor<
        std::ratio<1>, wpi::units::traits::dimension_of_t<Distance>>;
    using BaseFeedforward = wpi::math::SimpleMotorFeedforward<BaseUnit>;
    wpi_proto_ProtobufSimpleMotorFeedforward msg;
    if (!stream.Decode(msg)) {
      return {};
    }

    return wpi::math::SimpleMotorFeedforward<Distance>{
        wpi::units::volts<>{msg.ks},
        wpi::units::unit<typename BaseFeedforward::kv_unit>{msg.kv},
        wpi::units::unit<typename BaseFeedforward::ka_unit>{msg.ka},
        wpi::units::seconds<>{msg.dt},
    };
  }

  static bool Pack(OutputStream& stream,
                   const wpi::math::SimpleMotorFeedforward<Distance>& value) {
    using BaseUnit = wpi::units::conversion_factor<
        std::ratio<1>, wpi::units::traits::dimension_of_t<Distance>>;
    using BaseFeedforward = wpi::math::SimpleMotorFeedforward<BaseUnit>;
    wpi_proto_ProtobufSimpleMotorFeedforward msg{
        .ks = value.GetKs().value(),
        .kv = wpi::units::unit<typename BaseFeedforward::kv_unit>{value.GetKv()}
                  .value(),
        .ka = wpi::units::unit<typename BaseFeedforward::ka_unit>{value.GetKa()}
                  .value(),
        .dt = wpi::units::seconds<>{value.GetDt()}.value(),
    };
    return stream.Encode(msg);
  }
};
