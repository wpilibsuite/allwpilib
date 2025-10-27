// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <fmt/format.h>
#include <wpi/util/ct_string.hpp>
#include <wpi/util/struct/Struct.hpp>

#include "wpi/math/kinematics/SwerveDriveKinematics.hpp"

template <size_t NumModules>
struct wpi::util::Struct<wpi::math::SwerveDriveKinematics<NumModules>> {
  static constexpr ct_string kTypeName =
      wpi::util::Concat("SwerveDriveKinematics__"_ct_string,
                        wpi::util::NumToCtString<NumModules>());
  static constexpr std::string_view GetTypeName() { return kTypeName; }
  static constexpr size_t GetSize() {
    return NumModules * wpi::util::Struct<wpi::math::Translation2d>::GetSize();
  }
  static constexpr ct_string kSchema =
      wpi::util::Concat("Translation2d modules["_ct_string,
                        wpi::util::NumToCtString<NumModules>(), "]"_ct_string);
  static constexpr std::string_view GetSchema() { return kSchema; }

  static wpi::math::SwerveDriveKinematics<NumModules> Unpack(
      std::span<const uint8_t> data) {
    constexpr size_t kModulesOff = 0;
    return wpi::math::SwerveDriveKinematics<NumModules>{
        wpi::util::UnpackStructArray<wpi::math::Translation2d, kModulesOff,
                                     NumModules>(data)};
  }

  static void Pack(std::span<uint8_t> data,
                   const wpi::math::SwerveDriveKinematics<NumModules>& value) {
    constexpr size_t kModulesOff = 0;
    wpi::util::PackStructArray<kModulesOff, NumModules>(data,
                                                        value.GetModules());
  }

  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::util::ForEachStructSchema<wpi::math::Translation2d>(fn);
  }
};

static_assert(
    wpi::util::StructSerializable<wpi::math::SwerveDriveKinematics<4>>);
static_assert(wpi::util::HasNestedStruct<wpi::math::SwerveDriveKinematics<4>>);
static_assert(
    wpi::util::StructSerializable<wpi::math::SwerveDriveKinematics<3>>);
static_assert(wpi::util::HasNestedStruct<wpi::math::SwerveDriveKinematics<3>>);
