// Copyright (c) Sleipnir contributors

#pragma once

#include <utility>

#include <Eigen/SparseCore>

#include "sleipnir/autodiff/jacobian.hpp"
#include "sleipnir/autodiff/variable.hpp"
#include "sleipnir/autodiff/variable_matrix.hpp"
#include "sleipnir/util/concepts.hpp"
#include "sleipnir/util/symbol_exports.hpp"

namespace slp {

/// This class calculates the gradient of a variable with respect to a vector of
/// variables.
///
/// The gradient is only recomputed if the variable expression is quadratic or
/// higher order.
///
/// @tparam Scalar Scalar type.
template <typename Scalar>
class Gradient {
 public:
  /// Constructs a Gradient object.
  ///
  /// @param variable Variable of which to compute the gradient.
  /// @param wrt Variable with respect to which to compute the gradient.
  Gradient(Variable<Scalar> variable, Variable<Scalar> wrt)
      : m_jacobian{std::move(variable), std::move(wrt)} {}

  /// Constructs a Gradient object.
  ///
  /// @param variable Variable of which to compute the gradient.
  /// @param wrt Vector of variables with respect to which to compute the
  ///     gradient.
  Gradient(Variable<Scalar> variable, SleipnirMatrixLike<Scalar> auto wrt)
      : m_jacobian{VariableMatrix{std::move(variable)}, std::move(wrt)} {}

  /// Returns the gradient as a VariableMatrix.
  ///
  /// This is useful when constructing optimization problems with derivatives in
  /// them.
  ///
  /// @return The gradient as a VariableMatrix.
  VariableMatrix<Scalar> get() const { return m_jacobian.get().T(); }

  /// Evaluates the gradient at wrt's value.
  ///
  /// @return The gradient at wrt's value.
  const Eigen::SparseVector<Scalar>& value() {
    m_g = m_jacobian.value().transpose();

    return m_g;
  }

 private:
  Eigen::SparseVector<Scalar> m_g;

  Jacobian<Scalar> m_jacobian;
};

extern template class EXPORT_TEMPLATE_DECLARE(SLEIPNIR_DLLEXPORT)
Gradient<double>;

}  // namespace slp
