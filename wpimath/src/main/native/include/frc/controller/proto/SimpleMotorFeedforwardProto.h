// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/protobuf/Protobuf.h>

#include "frc/controller/SimpleMotorFeedforward.h"
#include "pb.h"
#include "units/length.h"
#include "wpimath/protobuf/controller.npb.h"

// Everything is converted into units for
// frc::SimpleMotorFeedforward<units::meters>

template <class Distance>
struct wpi::Protobuf<frc::SimpleMotorFeedforward<Distance>> {  // NOLINT
  using MessageStruct = wpi_proto_ProtobufSimpleMotorFeedforward;
  using InputStream =
      wpi::ProtoInputStream<frc::SimpleMotorFeedforward<Distance>>;
  using OutputStream =
      wpi::ProtoOutputStream<frc::SimpleMotorFeedforward<Distance>>;

  static std::optional<frc::SimpleMotorFeedforward<Distance>> Unpack(
      InputStream& stream) {
    wpi_proto_ProtobufSimpleMotorFeedforward msg;
    if (!stream.Decode(msg)) {
      return {};
    }

    return frc::SimpleMotorFeedforward<Distance>{
        units::volt_t{msg.ks},
        units::unit_t<frc::SimpleMotorFeedforward<units::meters>::kv_unit>{
            msg.kv},
        units::unit_t<frc::SimpleMotorFeedforward<units::meters>::ka_unit>{
            msg.ka},
        units::second_t{msg.dt},
    };
  }

  static bool Pack(OutputStream& stream,
                   const frc::SimpleMotorFeedforward<Distance>& value) {
    wpi_proto_ProtobufSimpleMotorFeedforward msg{
        .ks = value.GetKs().value(),
        .kv =
            units::unit_t<frc::SimpleMotorFeedforward<units::meters>::kv_unit>{
                value.GetKv()}
                .value(),
        .ka =
            units::unit_t<frc::SimpleMotorFeedforward<units::meters>::ka_unit>{
                value.GetKa()}
                .value(),
        .dt = units::second_t{value.GetDt()}.value(),
    };
    return stream.Encode(msg);
  }
};
