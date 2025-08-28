// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdexcept>

#include <fmt/format.h>
#include <wpi/protobuf/Protobuf.h>
#include <wpi/protobuf/ProtobufCallbacks.h>

#include "wpimath/kinematics/SwerveDriveKinematics.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <size_t NumModules>
struct wpi::Protobuf<wpimath::SwerveDriveKinematics<NumModules>> {
  using MessageStruct = wpi_proto_ProtobufSwerveDriveKinematics;
  using InputStream =
      wpi::ProtoInputStream<wpimath::SwerveDriveKinematics<NumModules>>;
  using OutputStream =
      wpi::ProtoOutputStream<wpimath::SwerveDriveKinematics<NumModules>>;

  static std::optional<wpimath::SwerveDriveKinematics<NumModules>> Unpack(
      InputStream& stream) {
    wpi::WpiArrayUnpackCallback<wpimath::Translation2d, NumModules> modules;
    wpi_proto_ProtobufSwerveDriveKinematics msg{
        .modules = modules.Callback(),
    };
    modules.SetLimits(wpi::DecodeLimits::Fail);
    if (!stream.Decode(msg)) {
      return {};
    }

    return wpimath::SwerveDriveKinematics<NumModules>{modules.Array()};
  }

  static bool Pack(OutputStream& stream,
                   const wpimath::SwerveDriveKinematics<NumModules>& value) {
    wpi::PackCallback<wpimath::Translation2d> modules{value.GetModules()};
    wpi_proto_ProtobufSwerveDriveKinematics msg{
        .modules = modules.Callback(),
    };
    return stream.Encode(msg);
  }
};
