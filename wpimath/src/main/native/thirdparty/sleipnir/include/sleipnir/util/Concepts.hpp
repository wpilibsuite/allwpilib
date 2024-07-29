// Copyright (c) Sleipnir contributors

#pragma once

#include <concepts>

#include <Eigen/Core>

#include "sleipnir/autodiff/Variable.hpp"
#include "sleipnir/autodiff/VariableMatrix.hpp"

namespace sleipnir {

template <typename T>
concept ScalarLike = std::same_as<T, double> || std::same_as<T, int> ||
                     std::same_as<T, Variable>;

template <typename T>
concept SleipnirMatrixLike = std::same_as<T, VariableMatrix> ||
                             std::same_as<T, VariableBlock<VariableMatrix>>;

template <typename Derived>
concept EigenMatrixLike =
    std::derived_from<Derived, Eigen::MatrixBase<Derived>>;

template <typename T>
concept EigenSolver = requires(T t) { t.solve(Eigen::VectorXd{}); };

template <typename T>
concept MatrixLike = SleipnirMatrixLike<T> || EigenMatrixLike<T>;

}  // namespace sleipnir
