// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/protobuf/Protobuf.h>

#include "controller.npb.h"
#include "frc/controller/SimpleMotorFeedforward.h"
#include "units/length.h"
#include "pb.h"

// Everything is converted into units for
// frc::SimpleMotorFeedforward<units::meters>

template <class Distance>
struct wpi::Protobuf<frc::SimpleMotorFeedforward<Distance>> {
  const pb_msgdesc_t* Message() {
    return get_wpi_proto_ProtobufSimpleMotorFeedforward_msg();
  }

  static std::optional<frc::SimpleMotorFeedforward<Distance>> Unpack(
      wpi::ProtoInputStream& stream) {
    wpi_proto_ProtobufSimpleMotorFeedforward msg;
    if (!stream.DecodeNoInit(msg)) {
      return {};
    }

    return {
        units::volt_t{msg.ks},
        units::unit_t<frc::SimpleMotorFeedforward<units::meters>::kv_unit>{
            msg.kv},
        units::unit_t<frc::SimpleMotorFeedforward<units::meters>::ka_unit>{
            msg.ka},
        units::second_t{msg.dt},
    };
  }

  static bool Pack(wpi::ProtoOutputStream& stream,
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
