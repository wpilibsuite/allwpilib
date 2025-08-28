// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdexcept>

#include <fmt/format.h>
#include <wpi/math/kinematics/SwerveDriveKinematics.hpp>
#include <wpi/protobuf/Protobuf.h>
#include <wpi/protobuf/ProtobufCallbacks.h>

#include "wpimath/protobuf/kinematics.npb.hpp"

template <size_t NumModules>
struct wpi::Protobuf<wpi::math::SwerveDriveKinematics<NumModules>> {
  using MessageStruct = wpi_proto_ProtobufSwerveDriveKinematics;
  using InputStream =
      wpi::ProtoInputStream<wpi::math::SwerveDriveKinematics<NumModules>>;
  using OutputStream =
      wpi::ProtoOutputStream<wpi::math::SwerveDriveKinematics<NumModules>>;

  static std::optional<wpi::math::SwerveDriveKinematics<NumModules>> Unpack(
      InputStream& stream) {
    wpi::WpiArrayUnpackCallback<wpi::math::Translation2d, NumModules> modules;
    wpi_proto_ProtobufSwerveDriveKinematics msg{
        .modules = modules.Callback(),
    };
    modules.SetLimits(wpi::DecodeLimits::Fail);
    if (!stream.Decode(msg)) {
      return {};
    }

    return wpi::math::SwerveDriveKinematics<NumModules>{modules.Array()};
  }

  static bool Pack(OutputStream& stream,
                   const wpi::math::SwerveDriveKinematics<NumModules>& value) {
    wpi::PackCallback<wpi::math::Translation2d> modules{value.GetModules()};
    wpi_proto_ProtobufSwerveDriveKinematics msg{
        .modules = modules.Callback(),
    };
    return stream.Encode(msg);
  }
};
