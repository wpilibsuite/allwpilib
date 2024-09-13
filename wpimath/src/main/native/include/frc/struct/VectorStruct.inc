// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/struct/VectorStruct.h"

template <int Size, int Options, int MaxRows, int MaxCols>
frc::Matrixd<Size, 1, Options, MaxRows, MaxCols>
wpi::Struct<frc::Matrixd<Size, 1, Options, MaxRows, MaxCols>>::Unpack(
    std::span<const uint8_t> data) {
  constexpr size_t kDataOff = 0;
  wpi::array<double, Size> vec_data =
      wpi::UnpackStructArray<double, kDataOff, Size>(data);
  frc::Matrixd<Size, 1, Options, MaxRows, MaxCols> vec;
  for (int i = 0; i < Size; i++) {
    vec(i) = vec_data[i];
  }
  return vec;
}

template <int Size, int Options, int MaxRows, int MaxCols>
void wpi::Struct<frc::Matrixd<Size, 1, Options, MaxRows, MaxCols>>::Pack(
    std::span<uint8_t> data,
    const frc::Matrixd<Size, 1, Options, MaxRows, MaxCols>& value) {
  constexpr size_t kDataOff = 0;
  wpi::array<double, Size> vec_data(wpi::empty_array);
  for (int i = 0; i < Size; i++) {
    vec_data[i] = value(i);
  }
  wpi::PackStructArray<kDataOff, Size>(data, vec_data);
}
