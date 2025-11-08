// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <fmt/format.h>

#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/util/ct_string.hpp"
#include "wpi/util/struct/Struct.hpp"

template <int Rows, int Cols, int Options, int MaxRows, int MaxCols>
  requires(Cols != 1)
struct wpi::util::Struct<wpi::math::Matrixd<Rows, Cols, Options, MaxRows, MaxCols>> {
  static constexpr ct_string kTypeName =
      wpi::util::Concat("Matrix__"_ct_string, wpi::util::NumToCtString<Rows>(),
                  "_"_ct_string, wpi::util::NumToCtString<Cols>());
  static constexpr std::string_view GetTypeName() { return kTypeName; }
  static constexpr size_t GetSize() { return Rows * Cols * 8; }
  static constexpr ct_string kSchema =
      wpi::util::Concat("double data["_ct_string, wpi::util::NumToCtString<Rows * Cols>(),
                  "]"_ct_string);
  static constexpr std::string_view GetSchema() { return kSchema; }

  static wpi::math::Matrixd<Rows, Cols, Options, MaxRows, MaxCols> Unpack(
      std::span<const uint8_t> data) {
    constexpr size_t kDataOff = 0;
    wpi::util::array<double, Rows * Cols> mat_data =
        wpi::util::UnpackStructArray<double, kDataOff, Rows * Cols>(data);
    wpi::math::Matrixd<Rows, Cols, Options, MaxRows, MaxCols> mat;
    for (int i = 0; i < Rows * Cols; i++) {
      mat(i) = mat_data[i];
    }
    return mat;
  }

  static void Pack(
      std::span<uint8_t> data,
      const wpi::math::Matrixd<Rows, Cols, Options, MaxRows, MaxCols>& value) {
    constexpr size_t kDataOff = 0;
    wpi::util::array<double, Rows * Cols> mat_data(wpi::util::empty_array);
    for (int i = 0; i < Rows * Cols; i++) {
      mat_data[i] = value(i);
    }
    wpi::util::PackStructArray<kDataOff, Rows * Cols>(data, mat_data);
  }
};

static_assert(wpi::util::StructSerializable<wpi::math::Matrixd<1, 2>>);
static_assert(wpi::util::StructSerializable<wpi::math::Matrixd<3, 3>>);
