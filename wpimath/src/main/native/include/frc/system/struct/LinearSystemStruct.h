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
  static constexpr ct_string TYPE_NAME =
      wpi::Concat("LinearSystem__"_ct_string, wpi::NumToCtString<States>(),
                  "_"_ct_string, wpi::NumToCtString<Inputs>(), "_"_ct_string,
                  wpi::NumToCtString<Outputs>());
  static constexpr std::string_view GetTypeName() { return TYPE_NAME; }
  static constexpr size_t GetSize() {
    return wpi::Struct<frc::Matrixd<States, States>>::GetSize() +
           wpi::Struct<frc::Matrixd<States, Inputs>>::GetSize() +
           wpi::Struct<frc::Matrixd<Outputs, States>>::GetSize() +
           wpi::Struct<frc::Matrixd<Outputs, Inputs>>::GetSize();
  }
  static constexpr ct_string SCHEMA = wpi::Concat(
      wpi::Struct<frc::Matrixd<States, States>>::TYPE_NAME, " a;"_ct_string,
      wpi::Struct<frc::Matrixd<States, Inputs>>::TYPE_NAME, " b;"_ct_string,
      wpi::Struct<frc::Matrixd<Outputs, States>>::TYPE_NAME, " c;"_ct_string,
      wpi::Struct<frc::Matrixd<Outputs, Inputs>>::TYPE_NAME, " d"_ct_string);
  static constexpr std::string_view GetSchema() { return SCHEMA; }

  static frc::LinearSystem<States, Inputs, Outputs> Unpack(
      std::span<const uint8_t> data) {
    constexpr size_t A_OFF = 0;
    constexpr size_t B_OFF =
        A_OFF + wpi::GetStructSize<frc::Matrixd<States, States>>();
    constexpr size_t C_OFF =
        B_OFF + wpi::GetStructSize<frc::Matrixd<States, Inputs>>();
    constexpr size_t D_OFF =
        C_OFF + wpi::GetStructSize<frc::Matrixd<Outputs, States>>();
    return frc::LinearSystem<States, Inputs, Outputs>{
        wpi::UnpackStruct<frc::Matrixd<States, States>, A_OFF>(data),
        wpi::UnpackStruct<frc::Matrixd<States, Inputs>, B_OFF>(data),
        wpi::UnpackStruct<frc::Matrixd<Outputs, States>, C_OFF>(data),
        wpi::UnpackStruct<frc::Matrixd<Outputs, Inputs>, D_OFF>(data)};
  }

  static void Pack(std::span<uint8_t> data,
                   const frc::LinearSystem<States, Inputs, Outputs>& value) {
    constexpr size_t A_OFF = 0;
    constexpr size_t B_OFF =
        A_OFF + wpi::GetStructSize<frc::Matrixd<States, States>>();
    constexpr size_t C_OFF =
        B_OFF + wpi::GetStructSize<frc::Matrixd<States, Inputs>>();
    constexpr size_t D_OFF =
        C_OFF + wpi::GetStructSize<frc::Matrixd<Outputs, States>>();
    wpi::PackStruct<A_OFF>(data, value.A());
    wpi::PackStruct<B_OFF>(data, value.B());
    wpi::PackStruct<C_OFF>(data, value.C());
    wpi::PackStruct<D_OFF>(data, value.D());
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
