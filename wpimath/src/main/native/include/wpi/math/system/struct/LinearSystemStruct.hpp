// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/linalg/struct/StructProto.hpp"
#include "wpi/math/system/LinearSystem.hpp"
#include "wpi/util/ct_string.hpp"
#include "wpi/util/struct/Struct.hpp"

template <int States, int Inputs, int Outputs>
struct wpi::util::Struct<wpi::math::LinearSystem<States, Inputs, Outputs>> {
  static constexpr ct_string TYPE_NAME = wpi::util::Concat(
      "LinearSystem__"_ct_string, wpi::util::NumToCtString<States>(),
      "_"_ct_string, wpi::util::NumToCtString<Inputs>(), "_"_ct_string,
      wpi::util::NumToCtString<Outputs>());
  static constexpr std::string_view GetTypeName() { return TYPE_NAME; }
  static constexpr size_t GetSize() {
    return wpi::util::Struct<wpi::math::Matrixd<States, States>>::GetSize() +
           wpi::util::Struct<wpi::math::Matrixd<States, Inputs>>::GetSize() +
           wpi::util::Struct<wpi::math::Matrixd<Outputs, States>>::GetSize() +
           wpi::util::Struct<wpi::math::Matrixd<Outputs, Inputs>>::GetSize();
  }
  static constexpr ct_string SCHEMA = wpi::util::Concat(
      wpi::util::Struct<wpi::math::Matrixd<States, States>>::TYPE_NAME,
      " a;"_ct_string,
      wpi::util::Struct<wpi::math::Matrixd<States, Inputs>>::TYPE_NAME,
      " b;"_ct_string,
      wpi::util::Struct<wpi::math::Matrixd<Outputs, States>>::TYPE_NAME,
      " c;"_ct_string,
      wpi::util::Struct<wpi::math::Matrixd<Outputs, Inputs>>::TYPE_NAME,
      " d"_ct_string);
  static constexpr std::string_view GetSchema() { return SCHEMA; }

  static wpi::math::LinearSystem<States, Inputs, Outputs> Unpack(
      std::span<const uint8_t> data) {
    constexpr size_t A_OFF = 0;
    constexpr size_t B_OFF =
        A_OFF + wpi::util::GetStructSize<wpi::math::Matrixd<States, States>>();
    constexpr size_t C_OFF =
        B_OFF + wpi::util::GetStructSize<wpi::math::Matrixd<States, Inputs>>();
    constexpr size_t D_OFF =
        C_OFF + wpi::util::GetStructSize<wpi::math::Matrixd<Outputs, States>>();
    return wpi::math::LinearSystem<States, Inputs, Outputs>{
        wpi::util::UnpackStruct<wpi::math::Matrixd<States, States>, A_OFF>(
            data),
        wpi::util::UnpackStruct<wpi::math::Matrixd<States, Inputs>, B_OFF>(
            data),
        wpi::util::UnpackStruct<wpi::math::Matrixd<Outputs, States>, C_OFF>(
            data),
        wpi::util::UnpackStruct<wpi::math::Matrixd<Outputs, Inputs>, D_OFF>(
            data)};
  }

  static void Pack(
      std::span<uint8_t> data,
      const wpi::math::LinearSystem<States, Inputs, Outputs>& value) {
    constexpr size_t A_OFF = 0;
    constexpr size_t B_OFF =
        A_OFF + wpi::util::GetStructSize<wpi::math::Matrixd<States, States>>();
    constexpr size_t C_OFF =
        B_OFF + wpi::util::GetStructSize<wpi::math::Matrixd<States, Inputs>>();
    constexpr size_t D_OFF =
        C_OFF + wpi::util::GetStructSize<wpi::math::Matrixd<Outputs, States>>();
    wpi::util::PackStruct<A_OFF>(data, value.A());
    wpi::util::PackStruct<B_OFF>(data, value.B());
    wpi::util::PackStruct<C_OFF>(data, value.C());
    wpi::util::PackStruct<D_OFF>(data, value.D());
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
