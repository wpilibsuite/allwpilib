// Copyright (c) Sleipnir contributors

#pragma once

#include <concepts>
#include <type_traits>

#include <Eigen/Core>

namespace slp {

template <typename T>
concept ScalarLike = requires(std::decay_t<T> t) {
  t + 1.0;
  t = 1.0;
};

template <typename T>
concept SleipnirScalarLike = requires(std::decay_t<T> t) {
  t + 1.0;
  t = 1.0;
  { t.value() } -> std::same_as<double>;
};

template <typename T>
concept EigenMatrixLike =
    std::derived_from<std::decay_t<T>, Eigen::MatrixBase<std::decay_t<T>>>;

template <typename T>
concept SleipnirMatrixLike = requires(std::decay_t<T> t, int rows, int cols) {
  t.rows();
  t.cols();
  { t.value() } -> std::same_as<Eigen::MatrixXd>;
} && !EigenMatrixLike<T>;

template <typename T>
concept SleipnirType = SleipnirScalarLike<T> || SleipnirMatrixLike<T>;

template <typename T>
concept MatrixLike = SleipnirMatrixLike<T> || EigenMatrixLike<T>;

}  // namespace slp
