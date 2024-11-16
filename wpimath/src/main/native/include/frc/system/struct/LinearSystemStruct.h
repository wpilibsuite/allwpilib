// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <fmt/format.h>
#include <wpi/ct_string.h>
#include <wpi/struct/Struct.h>

#include "frc/struct/MatrixStruct.h"
#include "frc/system/LinearSystem.h"

template <int States, int Inputs, int Outputs>
struct wpi::Struct<frc::LinearSystem<States, Inputs, Outputs>> {
  static constexpr ct_string kTypeName =
      wpi::Concat("LinearSystem__"_ct_string, wpi::NumToCtString<States>(),
                  "_"_ct_string, wpi::NumToCtString<Inputs>(), "_"_ct_string,
                  wpi::NumToCtString<Outputs>());
  static constexpr std::string_view GetTypeName() { return kTypeName; }
  static constexpr size_t GetSize() {
    return wpi::Struct<frc::Matrixd<States, States>>::GetSize() +
           wpi::Struct<frc::Matrixd<States, Inputs>>::GetSize() +
           wpi::Struct<frc::Matrixd<Outputs, States>>::GetSize() +
           wpi::Struct<frc::Matrixd<Outputs, Inputs>>::GetSize();
  }
  static constexpr ct_string kSchema = wpi::Concat(
      wpi::Struct<frc::Matrixd<States, States>>::kTypeName, " a;"_ct_string,
      wpi::Struct<frc::Matrixd<States, Inputs>>::kTypeName, " b;"_ct_string,
      wpi::Struct<frc::Matrixd<Outputs, States>>::kTypeName, " c;"_ct_string,
      wpi::Struct<frc::Matrixd<Outputs, Inputs>>::kTypeName, " d"_ct_string);
  static constexpr std::string_view GetSchema() { return kSchema; }

  static frc::LinearSystem<States, Inputs, Outputs> Unpack(
      std::span<const uint8_t> data) {
    constexpr size_t kAOff = 0;
    constexpr size_t kBOff =
        kAOff + wpi::GetStructSize<frc::Matrixd<States, States>>();
    constexpr size_t kCOff =
        kBOff + wpi::GetStructSize<frc::Matrixd<States, Inputs>>();
    constexpr size_t kDOff =
        kCOff + wpi::GetStructSize<frc::Matrixd<Outputs, States>>();
    return frc::LinearSystem<States, Inputs, Outputs>{
        wpi::UnpackStruct<frc::Matrixd<States, States>, kAOff>(data),
        wpi::UnpackStruct<frc::Matrixd<States, Inputs>, kBOff>(data),
        wpi::UnpackStruct<frc::Matrixd<Outputs, States>, kCOff>(data),
        wpi::UnpackStruct<frc::Matrixd<Outputs, Inputs>, kDOff>(data)};
  }

  static void Pack(std::span<uint8_t> data,
                   const frc::LinearSystem<States, Inputs, Outputs>& value) {
    constexpr size_t kAOff = 0;
    constexpr size_t kBOff =
        kAOff + wpi::GetStructSize<frc::Matrixd<States, States>>();
    constexpr size_t kCOff =
        kBOff + wpi::GetStructSize<frc::Matrixd<States, Inputs>>();
    constexpr size_t kDOff =
        kCOff + wpi::GetStructSize<frc::Matrixd<Outputs, States>>();
    wpi::PackStruct<kAOff>(data, value.A());
    wpi::PackStruct<kBOff>(data, value.B());
    wpi::PackStruct<kCOff>(data, value.C());
    wpi::PackStruct<kDOff>(data, value.D());
  }

  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<frc::Matrixd<States, States>>(fn);
    wpi::ForEachStructSchema<frc::Matrixd<States, Inputs>>(fn);
    wpi::ForEachStructSchema<frc::Matrixd<Outputs, States>>(fn);
    wpi::ForEachStructSchema<frc::Matrixd<Outputs, Inputs>>(fn);
  }
};

static_assert(wpi::StructSerializable<frc::LinearSystem<4, 3, 2>>);
static_assert(wpi::HasNestedStruct<frc::LinearSystem<4, 3, 2>>);
static_assert(wpi::StructSerializable<frc::LinearSystem<2, 3, 4>>);
static_assert(wpi::HasNestedStruct<frc::LinearSystem<2, 3, 4>>);
