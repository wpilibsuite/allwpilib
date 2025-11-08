// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <fmt/format.h>
#include <wpi/ct_string.h>
#include <wpi/struct/Struct.h>

#include "frc/EigenCore.h"

template <int Rows, int Cols, int Options, int MaxRows, int MaxCols>
  requires(Cols != 1)
struct wpi::Struct<frc::Matrixd<Rows, Cols, Options, MaxRows, MaxCols>> {
  static constexpr ct_string kTypeName =
      wpi::Concat("Matrix__"_ct_string, wpi::NumToCtString<Rows>(),
                  "_"_ct_string, wpi::NumToCtString<Cols>());
  static constexpr std::string_view GetTypeName() { return kTypeName; }
  static constexpr size_t GetSize() { return Rows * Cols * 8; }
  static constexpr ct_string kSchema =
      wpi::Concat("double data["_ct_string, wpi::NumToCtString<Rows * Cols>(),
                  "]"_ct_string);
  static constexpr std::string_view GetSchema() { return kSchema; }

  static frc::Matrixd<Rows, Cols, Options, MaxRows, MaxCols> Unpack(
      std::span<const uint8_t> data) {
    constexpr size_t kDataOff = 0;
    wpi::array<double, Rows * Cols> mat_data =
        wpi::UnpackStructArray<double, kDataOff, Rows * Cols>(data);
    frc::Matrixd<Rows, Cols, Options, MaxRows, MaxCols> mat;
    for (int i = 0; i < Rows * Cols; i++) {
      mat(i) = mat_data[i];
    }
    return mat;
  }

  static void Pack(
      std::span<uint8_t> data,
      const frc::Matrixd<Rows, Cols, Options, MaxRows, MaxCols>& value) {
    constexpr size_t kDataOff = 0;
    wpi::array<double, Rows * Cols> mat_data(wpi::empty_array);
    for (int i = 0; i < Rows * Cols; i++) {
      mat_data[i] = value(i);
    }
    wpi::PackStructArray<kDataOff, Rows * Cols>(data, mat_data);
  }
};

static_assert(wpi::StructSerializable<frc::Matrixd<1, 2>>);
static_assert(wpi::StructSerializable<frc::Matrixd<3, 3>>);
