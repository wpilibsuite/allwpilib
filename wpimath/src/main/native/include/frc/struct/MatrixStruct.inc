// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/struct/MatrixStruct.h"

template <int Rows, int Cols, int Options, int MaxRows, int MaxCols>
  requires(Cols != 1)
frc::Matrixd<Rows, Cols, Options, MaxRows, MaxCols>
wpi::Struct<frc::Matrixd<Rows, Cols, Options, MaxRows, MaxCols>>::Unpack(
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

template <int Rows, int Cols, int Options, int MaxRows, int MaxCols>
  requires(Cols != 1)
void wpi::Struct<frc::Matrixd<Rows, Cols, Options, MaxRows, MaxCols>>::Pack(
    std::span<uint8_t> data,
    const frc::Matrixd<Rows, Cols, Options, MaxRows, MaxCols>& value) {
  constexpr size_t kDataOff = 0;
  wpi::array<double, Rows * Cols> mat_data(wpi::empty_array);
  for (int i = 0; i < Rows * Cols; i++) {
    mat_data[i] = value(i);
  }
  wpi::PackStructArray<kDataOff, Rows * Cols>(data, mat_data);
}
