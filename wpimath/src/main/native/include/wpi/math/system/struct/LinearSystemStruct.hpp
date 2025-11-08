// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <fmt/format.h>

#include "wpi/math/linalg/struct/StructProto.hpp"
#include "wpi/math/system/LinearSystem.hpp"
#include "wpi/util/ct_string.hpp"
#include "wpi/util/struct/Struct.hpp"

template <int States, int Inputs, int Outputs>
struct wpi::util::Struct<wpi::math::LinearSystem<States, Inputs, Outputs>> {
  static constexpr ct_string kTypeName =
      wpi::util::Concat("LinearSystem__"_ct_string, wpi::util::NumToCtString<States>(),
                  "_"_ct_string, wpi::util::NumToCtString<Inputs>(), "_"_ct_string,
                  wpi::util::NumToCtString<Outputs>());
  static constexpr std::string_view GetTypeName() { return kTypeName; }
  static constexpr size_t GetSize() {
    return wpi::util::Struct<wpi::math::Matrixd<States, States>>::GetSize() +
           wpi::util::Struct<wpi::math::Matrixd<States, Inputs>>::GetSize() +
           wpi::util::Struct<wpi::math::Matrixd<Outputs, States>>::GetSize() +
           wpi::util::Struct<wpi::math::Matrixd<Outputs, Inputs>>::GetSize();
  }
  static constexpr ct_string kSchema = wpi::util::Concat(
      wpi::util::Struct<wpi::math::Matrixd<States, States>>::kTypeName, " a;"_ct_string,
      wpi::util::Struct<wpi::math::Matrixd<States, Inputs>>::kTypeName, " b;"_ct_string,
      wpi::util::Struct<wpi::math::Matrixd<Outputs, States>>::kTypeName, " c;"_ct_string,
      wpi::util::Struct<wpi::math::Matrixd<Outputs, Inputs>>::kTypeName, " d"_ct_string);
  static constexpr std::string_view GetSchema() { return kSchema; }

  static wpi::math::LinearSystem<States, Inputs, Outputs> Unpack(
      std::span<const uint8_t> data) {
    constexpr size_t kAOff = 0;
    constexpr size_t kBOff =
        kAOff + wpi::util::GetStructSize<wpi::math::Matrixd<States, States>>();
    constexpr size_t kCOff =
        kBOff + wpi::util::GetStructSize<wpi::math::Matrixd<States, Inputs>>();
    constexpr size_t kDOff =
        kCOff + wpi::util::GetStructSize<wpi::math::Matrixd<Outputs, States>>();
    return wpi::math::LinearSystem<States, Inputs, Outputs>{
        wpi::util::UnpackStruct<wpi::math::Matrixd<States, States>, kAOff>(data),
        wpi::util::UnpackStruct<wpi::math::Matrixd<States, Inputs>, kBOff>(data),
        wpi::util::UnpackStruct<wpi::math::Matrixd<Outputs, States>, kCOff>(data),
        wpi::util::UnpackStruct<wpi::math::Matrixd<Outputs, Inputs>, kDOff>(data)};
  }

  static void Pack(std::span<uint8_t> data,
                   const wpi::math::LinearSystem<States, Inputs, Outputs>& value) {
    constexpr size_t kAOff = 0;
    constexpr size_t kBOff =
        kAOff + wpi::util::GetStructSize<wpi::math::Matrixd<States, States>>();
    constexpr size_t kCOff =
        kBOff + wpi::util::GetStructSize<wpi::math::Matrixd<States, Inputs>>();
    constexpr size_t kDOff =
        kCOff + wpi::util::GetStructSize<wpi::math::Matrixd<Outputs, States>>();
    wpi::util::PackStruct<kAOff>(data, value.A());
    wpi::util::PackStruct<kBOff>(data, value.B());
    wpi::util::PackStruct<kCOff>(data, value.C());
    wpi::util::PackStruct<kDOff>(data, value.D());
  }

  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::util::ForEachStructSchema<wpi::math::Matrixd<States, States>>(fn);
    wpi::util::ForEachStructSchema<wpi::math::Matrixd<States, Inputs>>(fn);
    wpi::util::ForEachStructSchema<wpi::math::Matrixd<Outputs, States>>(fn);
    wpi::util::ForEachStructSchema<wpi::math::Matrixd<Outputs, Inputs>>(fn);
  }
};

static_assert(wpi::util::StructSerializable<wpi::math::LinearSystem<4, 3, 2>>);
static_assert(wpi::util::HasNestedStruct<wpi::math::LinearSystem<4, 3, 2>>);
static_assert(wpi::util::StructSerializable<wpi::math::LinearSystem<2, 3, 4>>);
static_assert(wpi::util::HasNestedStruct<wpi::math::LinearSystem<2, 3, 4>>);
