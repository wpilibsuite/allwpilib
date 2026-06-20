// Copyright (c) Sleipnir contributors

#pragma once

#include <algorithm>
#include <utility>

#include <Eigen/Core>
#include <Eigen/SparseCore>

#include "sleipnir/optimization/solver/util/sparse_inf_norms.hpp"

// See docs/algorithms.md#Works_cited for citation definitions

namespace slp {

/// Automatic problem scaling factors.
///
/// @tparam Scalar Scalar type.
template <typename Scalar>
struct ProblemScaling {
  /// Type alias for dense vector.
  using DenseVector = Eigen::Vector<Scalar, Eigen::Dynamic>;
  /// Type alias for sparse matrix.
  using SparseMatrix = Eigen::SparseMatrix<Scalar>;
  /// Type alias for sparse vector.
  using SparseVector = Eigen::SparseVector<Scalar>;

  /// Cost scaling factor d_f.
  Scalar f = Scalar(1);

  /// Equality constraint scaling factors d_cₑ.
  DenseVector c_e;

  /// Inequality constraint scaling factors d_cᵢ.
  DenseVector c_i;

  /// Constructs identity problem scaling.
  ProblemScaling() = default;

  /// Constructs problem scaling from explicit factors.
  ///
  /// @param f Cost scaling factor d_f.
  /// @param c_e Equality constraint scaling factors d_cₑ.
  /// @param c_i Inequality constraint scaling factors d_cᵢ.
  ProblemScaling(Scalar f, DenseVector c_e, DenseVector c_i)
      : f{f}, c_e{std::move(c_e)}, c_i{std::move(c_i)} {}

  /// Computes Newton problem scaling.
  ///
  /// Scales the cost so the largest gradient component at the starting point
  /// is at most gₘₐₓ:
  ///
  ///   d_f = min(1, gₘₐₓ / ‖∇f(x₀)‖_∞)
  ///
  /// See §3.8 Automatic Scaling of the Problem Statement in [2].
  ///
  /// @param g Cost gradient ∇f, evaluated at the starting point.
  explicit ProblemScaling(const DenseVector& g) {
    constexpr Scalar g_max(100);

    f = std::min(Scalar(1), g_max / g.template lpNorm<Eigen::Infinity>());
  }

  /// Computes SQP problem scaling.
  ///
  /// Scales the cost and each equality constraint so the largest gradient
  /// component at the starting point is at most gₘₐₓ:
  ///
  ///   d_f     = min(1, gₘₐₓ / ‖∇f(x₀)‖_∞)
  ///   d_cₑ[j] = min(1, gₘₐₓ / ‖∇cₑⱼ(x₀)‖_∞)
  ///
  /// See §3.8 Automatic Scaling of the Problem Statement in [2].
  ///
  /// @param g Cost gradient ∇f, evaluated at the starting point.
  /// @param A_e Equality constraint Jacobian Aₑ, evaluated at the starting
  ///     point.
  ProblemScaling(const DenseVector& g, const SparseMatrix& A_e) {
    constexpr Scalar g_max(100);

    f = std::min(Scalar(1), g_max / g.template lpNorm<Eigen::Infinity>());
    c_e = (g_max / sparse_inf_norms(A_e).array()).min(Scalar(1)).matrix();
  }

  /// Computes interior-point problem scaling.
  ///
  /// Scales the cost and each constraint so the largest gradient
  /// component at the starting point is at most gₘₐₓ:
  ///
  ///   d_f    = min(1, gₘₐₓ / ‖∇f(x₀)‖_∞)
  ///   d_c[j] = min(1, gₘₐₓ / ‖∇cⱼ(x₀)‖_∞)
  ///
  /// See §3.8 Automatic Scaling of the Problem Statement in [2].
  ///
  /// @param g Cost gradient ∇f, evaluated at the starting point.
  /// @param A_e Equality constraint Jacobian Aₑ, evaluated at the starting
  ///     point.
  /// @param A_i Inequality constraint Jacobian Aᵢ, evaluated at the starting
  ///     point.
  ProblemScaling(const DenseVector& g, const SparseMatrix& A_e,
                 const SparseMatrix& A_i) {
    constexpr Scalar g_max(100);

    f = std::min(Scalar(1), g_max / g.template lpNorm<Eigen::Infinity>());
    c_e = (g_max / sparse_inf_norms(A_e).array()).min(Scalar(1)).matrix();
    c_i = (g_max / sparse_inf_norms(A_i).array()).min(Scalar(1)).matrix();
  }

  /// Whether the problem scaling is identity.
  ///
  /// @return True if the problem scaling is identity.
  bool is_identity() const {
    return f == Scalar(1) && c_e.size() == 0 && c_i.size() == 0;
  }
};

}  // namespace slp
