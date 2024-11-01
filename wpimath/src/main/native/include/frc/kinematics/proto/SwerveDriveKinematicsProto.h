// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdexcept>

#include <fmt/format.h>
#include <wpi/protobuf/Protobuf.h>

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "frc/kinematics/SwerveDriveKinematics.h"
#include "kinematics.npb.h"

template <size_t NumModules>
struct wpi::Protobuf<frc::SwerveDriveKinematics<NumModules>> {
  static const pb_msgdesc_t* Message() {
    return get_wpi_proto_ProtobufSwerveDriveKinematics_msg();
  }

  static std::optional<frc::SwerveDriveKinematics<NumModules>> Unpack(
      wpi::ProtoInputStream& stream) {
    wpi::WpiArrayUnpackCallback<frc::Translation2d, NumModules> modules;
    wpi_proto_ProtobufSwerveDriveKinematics msg{
      .modules = modules.Callback(),
    };
    modules.SetLimits(wpi::DecodeLimits::Fail);
    if (!stream.DecodeNoInit(msg)) {
      return {};
    }

    return frc::SwerveDriveKinematics<NumModules>{modules.Array()};
  }

  static bool Pack(wpi::ProtoOutputStream& stream,
                   const frc::SwerveDriveKinematics<NumModules>& value) {
    wpi::PackCallback<frc::Translation2d> modules{value.GetModules()};
    wpi_proto_ProtobufSwerveDriveKinematics msg{
        .modules = modules.Callback(),
    };
    return stream.Encode(msg);
  }
};
