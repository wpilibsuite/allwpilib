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
  struct ArrayEmplaceWrapper {
    wpi::array<frc::Translation2d, NumModules> array;
    size_t current_index = 0;
  };

  static const pb_msgdesc_t* Message() {
    return get_wpi_proto_ProtobufSwerveDriveKinematics_msg();
  }

  static std::optional<frc::SwerveDriveKinematics<NumModules>> Unpack(
      wpi::ProtoInputStream& msg) {
    ArrayEmplaceWrapper modulesArray;
    wpi::DirectUnpackCallback<frc::Translation2d, ArrayEmplaceWrapper,
                              NumModules>
        modules{modulesArray};

    wpi_proto_ProtobufSwerveDriveKinematics msg {
      .modules = modules.Callback();
    };

    modules.SetLimits(wpi::DecodeLimits::Fail);

    if (!msg.DecodeNoInit(msg)) {
      return false;
    }
    return frc::SwerveDriveKinematics<NumModules>{modulesArray.array};
  }

  static bool Pack(wpi::ProtoOutputStream& stream,
                   const frc::SwerveDriveKinematics<NumModules>& value) {
    auto modulesArray = value.GetModules();
    wpi::PackCallback modules{&modulesArray};
    wpi_proto_ProtobufSwerveDriveKinematics msg {
      .modules = modules.Callback(),
    };
    return stream.Encode(msg);
  }
};
