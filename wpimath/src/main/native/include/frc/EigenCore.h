// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <Eigen/Core>

namespace frc {

template <int Size>
using Vectord = Eigen::Vector<double, Size>;

template <int Rows, int Cols,
          int Options = Eigen::AutoAlign |
                        ((Rows == 1 && Cols != 1) ? Eigen::RowMajor
                         : (Cols == 1 && Rows != 1)
                             ? Eigen::ColMajor
                             : EIGEN_DEFAULT_MATRIX_STORAGE_ORDER_OPTION),
          int MaxRows = Rows, int MaxCols = Cols>
using Matrixd = Eigen::Matrix<double, Rows, Cols, Options, MaxRows, MaxCols>;

}  // namespace frc
