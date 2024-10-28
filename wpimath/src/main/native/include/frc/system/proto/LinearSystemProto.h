// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdexcept>

#include <fmt/format.h>
#include <wpi/ProtoHelper.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/proto/MatrixProto.h"
#include "frc/system/LinearSystem.h"
#include "system.pb.h"

template <int States, int Inputs, int Outputs>
struct wpi::Protobuf<frc::LinearSystem<States, Inputs, Outputs>> {
  static google::protobuf::Message* New(google::protobuf::Arena* arena) {
    return wpi::CreateMessage<wpi::proto::ProtobufLinearSystem>(arena);
  }

  static frc::LinearSystem<States, Inputs, Outputs> Unpack(
      const google::protobuf::Message& msg) {
    auto m = static_cast<const wpi::proto::ProtobufLinearSystem*>(&msg);
    if (m->num_states() != States || m->num_inputs() != Inputs ||
        m->num_outputs() != Outputs) {
      throw std::invalid_argument(fmt::format(
          "Tried to unpack message with {} states and {} inputs and {} outputs "
          "into LinearSystem with {} states and {} inputs and {} outputs",
          m->num_states(), m->num_inputs(), m->num_outputs(), States, Inputs,
          Outputs));
    }
    return frc::LinearSystem<States, Inputs, Outputs>{
        wpi::UnpackProtobuf<frc::Matrixd<States, States>>(m->wpi_a()),
        wpi::UnpackProtobuf<frc::Matrixd<States, Inputs>>(m->wpi_b()),
        wpi::UnpackProtobuf<frc::Matrixd<Outputs, States>>(m->wpi_c()),
        wpi::UnpackProtobuf<frc::Matrixd<Outputs, Inputs>>(m->wpi_d())};
  }

  static void Pack(google::protobuf::Message* msg,
                   const frc::LinearSystem<States, Inputs, Outputs>& value) {
    auto m = static_cast<wpi::proto::ProtobufLinearSystem*>(msg);
    m->set_num_states(States);
    m->set_num_inputs(Inputs);
    m->set_num_outputs(Outputs);
    wpi::PackProtobuf(m->mutable_a(), value.A());
    wpi::PackProtobuf(m->mutable_b(), value.B());
    wpi::PackProtobuf(m->mutable_c(), value.C());
    wpi::PackProtobuf(m->mutable_d(), value.D());
  }
};
