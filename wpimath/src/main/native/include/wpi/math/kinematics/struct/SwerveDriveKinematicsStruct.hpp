// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <fmt/format.h>
#include <wpi/ct_string.h>
#include <wpi/struct/Struct.h>

#include "frc/kinematics/SwerveDriveKinematics.h"

template <size_t NumModules>
struct wpi::Struct<frc::SwerveDriveKinematics<NumModules>> {
  static constexpr ct_string kTypeName = wpi::Concat(
      "SwerveDriveKinematics__"_ct_string, wpi::NumToCtString<NumModules>());
  static constexpr std::string_view GetTypeName() { return kTypeName; }
  static constexpr size_t GetSize() {
    return NumModules * wpi::Struct<frc::Translation2d>::GetSize();
  }
  static constexpr ct_string kSchema =
      wpi::Concat("Translation2d modules["_ct_string,
                  wpi::NumToCtString<NumModules>(), "]"_ct_string);
  static constexpr std::string_view GetSchema() { return kSchema; }

  static frc::SwerveDriveKinematics<NumModules> Unpack(
      std::span<const uint8_t> data) {
    constexpr size_t kModulesOff = 0;
    return frc::SwerveDriveKinematics<NumModules>{
        wpi::UnpackStructArray<frc::Translation2d, kModulesOff, NumModules>(
            data)};
  }

  static void Pack(std::span<uint8_t> data,
                   const frc::SwerveDriveKinematics<NumModules>& value) {
    constexpr size_t kModulesOff = 0;
    wpi::PackStructArray<kModulesOff, NumModules>(data, value.GetModules());
  }

  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<frc::Translation2d>(fn);
  }
};

static_assert(wpi::StructSerializable<frc::SwerveDriveKinematics<4>>);
static_assert(wpi::HasNestedStruct<frc::SwerveDriveKinematics<4>>);
static_assert(wpi::StructSerializable<frc::SwerveDriveKinematics<3>>);
static_assert(wpi::HasNestedStruct<frc::SwerveDriveKinematics<3>>);
