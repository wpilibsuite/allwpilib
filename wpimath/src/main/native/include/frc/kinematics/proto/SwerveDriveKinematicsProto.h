// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdexcept>

#include <fmt/format.h>
#include <wpi/ProtoHelper.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/kinematics/SwerveDriveKinematics.h"
#include "kinematics.pb.h"

template <size_t NumModules>
struct wpi::Protobuf<frc::SwerveDriveKinematics<NumModules>> {
  static google::protobuf::Message* New(google::protobuf::Arena* arena) {
    return wpi::CreateMessage<wpi::proto::ProtobufSwerveDriveKinematics>(arena);
  }

  static frc::SwerveDriveKinematics<NumModules> Unpack(
      const google::protobuf::Message& msg) {
    auto m =
        static_cast<const wpi::proto::ProtobufSwerveDriveKinematics*>(&msg);
    if (m->modules_size() != NumModules) {
      throw std::invalid_argument(fmt::format(
          "Tried to unpack message with {} elements in modules into "
          "SwerveDriveKinematics with {} modules",
          m->modules_size(), NumModules));
    }
    return frc::SwerveDriveKinematics<NumModules>{
        wpi::UnpackProtobufArray<wpi::proto::ProtobufTranslation2d,
                                 frc::Translation2d, NumModules>(m->modules())};
  }

  static void Pack(google::protobuf::Message* msg,
                   const frc::SwerveDriveKinematics<NumModules>& value) {
    auto m = static_cast<wpi::proto::ProtobufSwerveDriveKinematics*>(msg);
    wpi::PackProtobufArray(m->mutable_modules(), value.GetModules());
  }
};
