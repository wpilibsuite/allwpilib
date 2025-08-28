// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <fmt/format.h>
#include <wpi/ct_string.h>
#include <wpi/struct/Struct.h>

#include "wpimath/struct/MatrixStruct.h"
#include "wpimath/system/LinearSystem.h"

template <int States, int Inputs, int Outputs>
struct wpi::Struct<wpimath::LinearSystem<States, Inputs, Outputs>> {
  static constexpr ct_string kTypeName =
      wpi::Concat("LinearSystem__"_ct_string, wpi::NumToCtString<States>(),
                  "_"_ct_string, wpi::NumToCtString<Inputs>(), "_"_ct_string,
                  wpi::NumToCtString<Outputs>());
  static constexpr std::string_view GetTypeName() { return kTypeName; }
  static constexpr size_t GetSize() {
    return wpi::Struct<wpimath::Matrixd<States, States>>::GetSize() +
           wpi::Struct<wpimath::Matrixd<States, Inputs>>::GetSize() +
           wpi::Struct<wpimath::Matrixd<Outputs, States>>::GetSize() +
           wpi::Struct<wpimath::Matrixd<Outputs, Inputs>>::GetSize();
  }
  static constexpr ct_string kSchema = wpi::Concat(
      wpi::Struct<wpimath::Matrixd<States, States>>::kTypeName, " a;"_ct_string,
      wpi::Struct<wpimath::Matrixd<States, Inputs>>::kTypeName, " b;"_ct_string,
      wpi::Struct<wpimath::Matrixd<Outputs, States>>::kTypeName,
      " c;"_ct_string,
      wpi::Struct<wpimath::Matrixd<Outputs, Inputs>>::kTypeName,
      " d"_ct_string);
  static constexpr std::string_view GetSchema() { return kSchema; }

  static wpimath::LinearSystem<States, Inputs, Outputs> Unpack(
      std::span<const uint8_t> data) {
    constexpr size_t kAOff = 0;
    constexpr size_t kBOff =
        kAOff + wpi::GetStructSize<wpimath::Matrixd<States, States>>();
    constexpr size_t kCOff =
        kBOff + wpi::GetStructSize<wpimath::Matrixd<States, Inputs>>();
    constexpr size_t kDOff =
        kCOff + wpi::GetStructSize<wpimath::Matrixd<Outputs, States>>();
    return wpimath::LinearSystem<States, Inputs, Outputs>{
        wpi::UnpackStruct<wpimath::Matrixd<States, States>, kAOff>(data),
        wpi::UnpackStruct<wpimath::Matrixd<States, Inputs>, kBOff>(data),
        wpi::UnpackStruct<wpimath::Matrixd<Outputs, States>, kCOff>(data),
        wpi::UnpackStruct<wpimath::Matrixd<Outputs, Inputs>, kDOff>(data)};
  }

  static void Pack(
      std::span<uint8_t> data,
      const wpimath::LinearSystem<States, Inputs, Outputs>& value) {
    constexpr size_t kAOff = 0;
    constexpr size_t kBOff =
        kAOff + wpi::GetStructSize<wpimath::Matrixd<States, States>>();
    constexpr size_t kCOff =
        kBOff + wpi::GetStructSize<wpimath::Matrixd<States, Inputs>>();
    constexpr size_t kDOff =
        kCOff + wpi::GetStructSize<wpimath::Matrixd<Outputs, States>>();
    wpi::PackStruct<kAOff>(data, value.A());
    wpi::PackStruct<kBOff>(data, value.B());
    wpi::PackStruct<kCOff>(data, value.C());
    wpi::PackStruct<kDOff>(data, value.D());
  }

  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<wpimath::Matrixd<States, States>>(fn);
    wpi::ForEachStructSchema<wpimath::Matrixd<States, Inputs>>(fn);
    wpi::ForEachStructSchema<wpimath::Matrixd<Outputs, States>>(fn);
    wpi::ForEachStructSchema<wpimath::Matrixd<Outputs, Inputs>>(fn);
  }
};

static_assert(wpi::StructSerializable<wpimath::LinearSystem<4, 3, 2>>);
static_assert(wpi::HasNestedStruct<wpimath::LinearSystem<4, 3, 2>>);
static_assert(wpi::StructSerializable<wpimath::LinearSystem<2, 3, 4>>);
static_assert(wpi::HasNestedStruct<wpimath::LinearSystem<2, 3, 4>>);
