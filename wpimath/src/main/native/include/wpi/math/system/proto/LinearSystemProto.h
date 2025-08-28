// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdexcept>
#include <utility>

#include <fmt/format.h>
#include <wpi/protobuf/Protobuf.h>
#include <wpi/protobuf/ProtobufCallbacks.h>

#include "wpi/math/proto/MatrixProto.h"
#include "wpimath/protobuf/system.npb.h"
#include "wpi/math/system/LinearSystem.h"

template <int States, int Inputs, int Outputs>
struct wpi::Protobuf<wpi::math::LinearSystem<States, Inputs, Outputs>> {
  using MessageStruct = wpi_proto_ProtobufLinearSystem;
  using InputStream =
      wpi::ProtoInputStream<wpi::math::LinearSystem<States, Inputs, Outputs>>;
  using OutputStream =
      wpi::ProtoOutputStream<wpi::math::LinearSystem<States, Inputs, Outputs>>;

  static std::optional<wpi::math::LinearSystem<States, Inputs, Outputs>> Unpack(
      InputStream& stream) {
    wpi::UnpackCallback<wpi::math::Matrixd<States, States>> a;
    a.SetLimits(wpi::DecodeLimits::Fail);
    wpi::UnpackCallback<wpi::math::Matrixd<States, Inputs>> b;
    a.SetLimits(wpi::DecodeLimits::Fail);
    wpi::UnpackCallback<wpi::math::Matrixd<Outputs, States>> c;
    a.SetLimits(wpi::DecodeLimits::Fail);
    wpi::UnpackCallback<wpi::math::Matrixd<Outputs, Inputs>> d;
    a.SetLimits(wpi::DecodeLimits::Fail);

    wpi_proto_ProtobufLinearSystem msg;
    msg.a = a.Callback();
    msg.b = b.Callback();
    msg.c = c.Callback();
    msg.d = d.Callback();

    if (!stream.Decode(msg)) {
      return {};
    }

    if (msg.num_inputs != Inputs || msg.num_outputs != Outputs ||
        msg.num_states != States) {
      return {};
    }

    auto ai = a.Items();
    auto bi = b.Items();
    auto ci = c.Items();
    auto di = d.Items();

    if (ai.empty() || bi.empty() || ci.empty() || di.empty()) {
      return {};
    }

    return wpi::math::LinearSystem<States, Inputs, Outputs>{
        std::move(ai[0]),
        std::move(bi[0]),
        std::move(ci[0]),
        std::move(di[0]),
    };
  }

  static bool Pack(
      OutputStream& stream,
      const wpi::math::LinearSystem<States, Inputs, Outputs>& value) {
    wpi::PackCallback a{&value.A()};
    wpi::PackCallback b{&value.B()};
    wpi::PackCallback c{&value.C()};
    wpi::PackCallback d{&value.D()};

    wpi_proto_ProtobufLinearSystem msg{
        .num_states = States,
        .num_inputs = Inputs,
        .num_outputs = Outputs,
        .a = a.Callback(),
        .b = b.Callback(),
        .c = c.Callback(),
        .d = d.Callback(),
    };

    return stream.Encode(msg);
  }
};
