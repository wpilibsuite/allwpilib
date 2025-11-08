// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <fmt/format.h>

#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/util/ct_string.hpp"
#include "wpi/util/struct/Struct.hpp"

template <int Size, int Options, int MaxRows, int MaxCols>
struct wpi::util::Struct<wpi::math::Matrixd<Size, 1, Options, MaxRows, MaxCols>> {
  static constexpr ct_string kTypeName =
      wpi::util::Concat("Vector__"_ct_string, wpi::util::NumToCtString<Size>());
  static constexpr std::string_view GetTypeName() { return kTypeName; }
  static constexpr size_t GetSize() { return Size * 8; }
  static constexpr ct_string kSchema = wpi::util::Concat(
      "double data["_ct_string, wpi::util::NumToCtString<Size>(), "]"_ct_string);
  static constexpr std::string_view GetSchema() { return kSchema; }

  static wpi::math::Matrixd<Size, 1, Options, MaxRows, MaxCols> Unpack(
      std::span<const uint8_t> data) {
    constexpr size_t kDataOff = 0;
    wpi::util::array<double, Size> vec_data =
        wpi::util::UnpackStructArray<double, kDataOff, Size>(data);
    wpi::math::Matrixd<Size, 1, Options, MaxRows, MaxCols> vec;
    for (int i = 0; i < Size; i++) {
      vec(i) = vec_data[i];
    }
    return vec;
  }

  static void Pack(
      std::span<uint8_t> data,
      const wpi::math::Matrixd<Size, 1, Options, MaxRows, MaxCols>& value) {
    constexpr size_t kDataOff = 0;
    wpi::util::array<double, Size> vec_data(wpi::util::empty_array);
    for (int i = 0; i < Size; i++) {
      vec_data[i] = value(i);
    }
    wpi::util::PackStructArray<kDataOff, Size>(data, vec_data);
  }
};

static_assert(wpi::util::StructSerializable<wpi::math::Vectord<1>>);
static_assert(wpi::util::StructSerializable<wpi::math::Vectord<3>>);
