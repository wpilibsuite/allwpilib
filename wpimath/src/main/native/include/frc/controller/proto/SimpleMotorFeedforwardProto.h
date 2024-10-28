// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/ProtoHelper.h>
#include <wpi/protobuf/Protobuf.h>

#include "controller.pb.h"
#include "frc/controller/SimpleMotorFeedforward.h"
#include "units/length.h"

// Everything is converted into units for
// frc::SimpleMotorFeedforward<units::meters>

template <class Distance>
struct wpi::Protobuf<frc::SimpleMotorFeedforward<Distance>> {
  static google::protobuf::Message* New(google::protobuf::Arena* arena) {
    return wpi::CreateMessage<wpi::proto::ProtobufSimpleMotorFeedforward>(
        arena);
  }

  static frc::SimpleMotorFeedforward<Distance> Unpack(
      const google::protobuf::Message& msg) {
    auto m =
        static_cast<const wpi::proto::ProtobufSimpleMotorFeedforward*>(&msg);
    return {units::volt_t{m->ks()},
            units::unit_t<frc::SimpleMotorFeedforward<units::meters>::kv_unit>{
                m->kv()},
            units::unit_t<frc::SimpleMotorFeedforward<units::meters>::ka_unit>{
                m->ka()},
            units::second_t{m->dt()}};
  }

  static void Pack(google::protobuf::Message* msg,
                   const frc::SimpleMotorFeedforward<Distance>& value) {
    auto m = static_cast<wpi::proto::ProtobufSimpleMotorFeedforward*>(msg);
    m->set_ks(value.GetKs().value());
    m->set_kv(
        units::unit_t<frc::SimpleMotorFeedforward<units::meters>::kv_unit>{
            value.GetKv()}
            .value());
    m->set_ka(
        units::unit_t<frc::SimpleMotorFeedforward<units::meters>::ka_unit>{
            value.GetKa()}
            .value());
    m->set_dt(units::second_t{value.GetDt()}.value());
  }
};
