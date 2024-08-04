// Copyright (c) Sleipnir contributors

#pragma once

#include <concepts>

#include <Eigen/Core>

namespace sleipnir {

template <typename T>
concept ScalarLike = requires(T t) {
  t + 1.0;
  t = 1.0;
};

template <typename T>
concept SleipnirMatrixLike = requires(T t, int rows, int cols) {
  t.Rows();
  t.Cols();
  t(rows, cols);
};

template <typename T>
concept EigenMatrixLike = std::derived_from<T, Eigen::MatrixBase<T>>;

template <typename T>
concept MatrixLike = SleipnirMatrixLike<T> || EigenMatrixLike<T>;

template <typename T>
concept EigenSolver = requires(T t) { t.solve(Eigen::VectorXd{}); };

}  // namespace sleipnir
