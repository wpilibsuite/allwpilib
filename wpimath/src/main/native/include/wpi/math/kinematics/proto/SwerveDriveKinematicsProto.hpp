// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdexcept>

#include <fmt/format.h>

#include "wpi/math/kinematics/SwerveDriveKinematics.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"
#include "wpimath/protobuf/protobuf_kinematics.npb.h"

template <size_t NumModules>
struct wpi::util::Protobuf<wpi::math::SwerveDriveKinematics<NumModules>> {
  using MessageStruct = wpi_proto_ProtobufSwerveDriveKinematics;
  using InputStream =
      wpi::util::ProtoInputStream<wpi::math::SwerveDriveKinematics<NumModules>>;
  using OutputStream = wpi::util::ProtoOutputStream<
      wpi::math::SwerveDriveKinematics<NumModules>>;

  static std::optional<wpi::math::SwerveDriveKinematics<NumModules>> Unpack(
      InputStream& stream) {
    wpi::util::WpiArrayUnpackCallback<wpi::math::Translation2d, NumModules>
        modules;
    wpi_proto_ProtobufSwerveDriveKinematics msg{
        .modules = modules.Callback(),
    };
    modules.SetLimits(wpi::util::DecodeLimits::Fail);
    if (!stream.Decode(msg)) {
      return {};
    }

    return wpi::math::SwerveDriveKinematics<NumModules>{modules.Array()};
  }

  static bool Pack(OutputStream& stream,
                   const wpi::math::SwerveDriveKinematics<NumModules>& value) {
    wpi::util::PackCallback<wpi::math::Translation2d> modules{
        value.GetModules()};
    wpi_proto_ProtobufSwerveDriveKinematics msg{
        .modules = modules.Callback(),
    };
    return stream.Encode(msg);
  }
};
