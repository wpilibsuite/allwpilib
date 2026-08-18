// Copyright (c) Sleipnir contributors

#pragma once

#include <algorithm>

#include <Eigen/Core>
#include <Eigen/SparseCore>

#include "sleipnir/optimization/solver/util/problem_scaling.hpp"

// See docs/algorithms.md#Works_cited for citation definitions

namespace slp {

/// Type of KKT error to compute.
enum class KKTErrorType {
  /// ∞-norm of scaled KKT condition errors.
  INF_NORM_SCALED,
  /// 1-norm of KKT condition errors.
  ONE_NORM
};

/// Returns the KKT error for Newton's method.
///
/// @tparam Scalar Scalar type.
/// @tparam T Type of KKT error to compute.
/// @param g Cost function gradient ∇f.
template <typename Scalar, KKTErrorType T>
Scalar kkt_error(const Eigen::Vector<Scalar, Eigen::Dynamic>& g) {
  // The KKT conditions from docs/algorithms.md:
  //
  //   ∇f = 0

  if constexpr (T == KKTErrorType::INF_NORM_SCALED) {
    return g.template lpNorm<Eigen::Infinity>();
  } else if constexpr (T == KKTErrorType::ONE_NORM) {
    return g.template lpNorm<1>();
  }
}

/// Returns the KKT error for Sequential Quadratic Programming.
///
/// @tparam Scalar Scalar type.
/// @tparam T Type of KKT error to compute.
/// @param g Cost function gradient ∇f.
/// @param A_e Equality constraint Jacobian Aₑ(x).
/// @param c_e Equality constraints cₑ(x).
/// @param y Equality constraint dual variables.
template <typename Scalar, KKTErrorType T>
Scalar kkt_error(const Eigen::Vector<Scalar, Eigen::Dynamic>& g,
                 const Eigen::SparseMatrix<Scalar>& A_e,
                 const Eigen::Vector<Scalar, Eigen::Dynamic>& c_e,
                 const Eigen::Vector<Scalar, Eigen::Dynamic>& y) {
  // The KKT conditions from docs/algorithms.md:
  //
  //   ∇f − Aₑᵀy = 0
  //   cₑ = 0

  if constexpr (T == KKTErrorType::INF_NORM_SCALED) {
    // See equation (5) of [2].

    // s_d = max(sₘₐₓ, ‖y‖₁ / m) / sₘₐₓ
    constexpr Scalar s_max(100);
    Scalar s_d =
        std::max(s_max, y.template lpNorm<1>() / Scalar(y.rows())) / s_max;

    // ‖∇f − Aₑᵀy‖_∞ / s_d
    // ‖cₑ‖_∞
    return std::max(
        {(g - A_e.transpose() * y).template lpNorm<Eigen::Infinity>() / s_d,
         c_e.template lpNorm<Eigen::Infinity>()});
  } else if constexpr (T == KKTErrorType::ONE_NORM) {
    return (g - A_e.transpose() * y).template lpNorm<1>() +
           c_e.template lpNorm<1>();
  }
}

/// Returns the KKT error for the interior-point method.
///
/// @tparam Scalar Scalar type.
/// @tparam T Type of KKT error to compute.
/// @param g Cost function gradient ∇f.
/// @param A_e Equality constraint Jacobian Aₑ(x).
/// @param c_e Equality constraints cₑ(x).
/// @param A_i Inequality constraint Jacobian Aᵢ(x).
/// @param c_i Inequality constraints cᵢ(x).
/// @param s Inequality constraint slack variables.
/// @param y Equality constraint dual variables.
/// @param z Inequality constraint dual variables.
/// @param μ Barrier parameter.
template <typename Scalar, KKTErrorType T>
Scalar kkt_error(const Eigen::Vector<Scalar, Eigen::Dynamic>& g,
                 const Eigen::SparseMatrix<Scalar>& A_e,
                 const Eigen::Vector<Scalar, Eigen::Dynamic>& c_e,
                 const Eigen::SparseMatrix<Scalar>& A_i,
                 const Eigen::Vector<Scalar, Eigen::Dynamic>& c_i,
                 const Eigen::Vector<Scalar, Eigen::Dynamic>& s,
                 const Eigen::Vector<Scalar, Eigen::Dynamic>& y,
                 const Eigen::Vector<Scalar, Eigen::Dynamic>& z, Scalar μ) {
  // The KKT conditions from docs/algorithms.md:
  //
  //   ∇f − Aₑᵀy − Aᵢᵀz = 0
  //   Sz − μe = 0
  //   cₑ = 0
  //   cᵢ − s = 0

  if constexpr (T == KKTErrorType::INF_NORM_SCALED) {
    // See equation (5) of [2].

    // s_d = max(sₘₐₓ, (‖y‖₁ + ‖z‖₁) / (m + n)) / sₘₐₓ
    constexpr Scalar s_max(100);
    Scalar s_d =
        std::max(s_max, (y.template lpNorm<1>() + z.template lpNorm<1>()) /
                            Scalar(y.rows() + z.rows())) /
        s_max;

    // s_c = max(sₘₐₓ, ‖z‖₁ / n) / sₘₐₓ
    Scalar s_c =
        std::max(s_max, z.template lpNorm<1>() / Scalar(z.rows())) / s_max;

    const auto S = s.asDiagonal();
    const Eigen::Vector<Scalar, Eigen::Dynamic> μe =
        Eigen::Vector<Scalar, Eigen::Dynamic>::Constant(s.rows(), μ);

    // ‖∇f − Aₑᵀy − Aᵢᵀz‖_∞ / s_d
    // ‖Sz − μe‖_∞ / s_c
    // ‖cₑ‖_∞
    // ‖cᵢ − s‖_∞
    return std::max({(g - A_e.transpose() * y - A_i.transpose() * z)
                             .template lpNorm<Eigen::Infinity>() /
                         s_d,
                     (S * z - μe).template lpNorm<Eigen::Infinity>() / s_c,
                     c_e.template lpNorm<Eigen::Infinity>(),
                     (c_i - s).template lpNorm<Eigen::Infinity>()});
  } else if constexpr (T == KKTErrorType::ONE_NORM) {
    const auto S = s.asDiagonal();
    const Eigen::Vector<Scalar, Eigen::Dynamic> μe =
        Eigen::Vector<Scalar, Eigen::Dynamic>::Constant(s.rows(), μ);

    return (g - A_e.transpose() * y - A_i.transpose() * z)
               .template lpNorm<1>() +
           (S * z - μe).template lpNorm<1>() + c_e.template lpNorm<1>() +
           (c_i - s).template lpNorm<1>();
  }
}

/// Returns the unscaled KKT error for Newton's method.
///
/// @tparam Scalar Scalar type.
/// @tparam T Type of KKT error to compute.
/// @param scaling Problem scaling.
/// @param g Scaled cost function gradient d_f·∇f.
template <typename Scalar, KKTErrorType T>
Scalar unscaled_kkt_error(const ProblemScaling<Scalar>& scaling,
                          const Eigen::Vector<Scalar, Eigen::Dynamic>& g) {
  using DenseVector = Eigen::Vector<Scalar, Eigen::Dynamic>;

  if (scaling.is_identity()) {
    return kkt_error<Scalar, T>(g);
  }

  const DenseVector g_unscaled = (Scalar(1) / scaling.f) * g;

  return kkt_error<Scalar, T>(g_unscaled);
}

/// Returns the unscaled KKT error for Sequential Quadratic Programming.
///
/// @tparam Scalar Scalar type.
/// @tparam T Type of KKT error to compute.
/// @param scaling Problem scaling.
/// @param g Scaled cost function gradient d_f·∇f.
/// @param A_e Scaled equality constraint Jacobian D_cₑ·Aₑ(x).
/// @param c_e Scaled equality constraints D_cₑ·cₑ(x).
/// @param y Scaled equality constraint dual variables.
template <typename Scalar, KKTErrorType T>
Scalar unscaled_kkt_error(const ProblemScaling<Scalar>& scaling,
                          const Eigen::Vector<Scalar, Eigen::Dynamic>& g,
                          const Eigen::SparseMatrix<Scalar>& A_e,
                          const Eigen::Vector<Scalar, Eigen::Dynamic>& c_e,
                          const Eigen::Vector<Scalar, Eigen::Dynamic>& y) {
  using DenseVector = Eigen::Vector<Scalar, Eigen::Dynamic>;
  using SparseMatrix = Eigen::SparseMatrix<Scalar>;

  if (scaling.is_identity()) {
    return kkt_error<Scalar, T>(g, A_e, c_e, y);
  }

  const Scalar inv_d_f = Scalar(1) / scaling.f;
  const DenseVector inv_d_c_e = scaling.c_e.cwiseInverse();

  const DenseVector g_unscaled = inv_d_f * g;
  const SparseMatrix A_e_unscaled = inv_d_c_e.asDiagonal() * A_e;
  const DenseVector c_e_unscaled = inv_d_c_e.cwiseProduct(c_e);
  const DenseVector y_unscaled = scaling.c_e.cwiseProduct(y) * inv_d_f;

  return kkt_error<Scalar, T>(g_unscaled, A_e_unscaled, c_e_unscaled,
                              y_unscaled);
}

/// Returns the unscaled KKT error for the interior-point method.
///
/// @tparam Scalar Scalar type.
/// @tparam T Type of KKT error to compute.
/// @param scaling Problem scaling.
/// @param g Scaled cost function gradient d_f·∇f.
/// @param A_e Scaled equality constraint Jacobian D_cₑ·Aₑ(x).
/// @param c_e Scaled equality constraints D_cₑ·cₑ(x).
/// @param A_i Scaled inequality constraint Jacobian D_cᵢ·Aᵢ(x).
/// @param c_i Scaled inequality constraints D_cᵢ·cᵢ(x).
/// @param s Scaled inequality constraint slack variables.
/// @param y Scaled equality constraint dual variables.
/// @param z Scaled inequality constraint dual variables.
/// @param μ Scaled barrier parameter.
template <typename Scalar, KKTErrorType T>
Scalar unscaled_kkt_error(const ProblemScaling<Scalar>& scaling,
                          const Eigen::Vector<Scalar, Eigen::Dynamic>& g,
                          const Eigen::SparseMatrix<Scalar>& A_e,
                          const Eigen::Vector<Scalar, Eigen::Dynamic>& c_e,
                          const Eigen::SparseMatrix<Scalar>& A_i,
                          const Eigen::Vector<Scalar, Eigen::Dynamic>& c_i,
                          const Eigen::Vector<Scalar, Eigen::Dynamic>& s,
                          const Eigen::Vector<Scalar, Eigen::Dynamic>& y,
                          const Eigen::Vector<Scalar, Eigen::Dynamic>& z,
                          Scalar μ) {
  using DenseVector = Eigen::Vector<Scalar, Eigen::Dynamic>;
  using SparseMatrix = Eigen::SparseMatrix<Scalar>;

  if (scaling.is_identity()) {
    return kkt_error<Scalar, T>(g, A_e, c_e, A_i, c_i, s, y, z, μ);
  }

  const Scalar inv_d_f = Scalar(1) / scaling.f;
  const DenseVector inv_d_c_e = scaling.c_e.cwiseInverse();
  const DenseVector inv_d_c_i = scaling.c_i.cwiseInverse();

  const DenseVector g_unscaled = inv_d_f * g;
  const SparseMatrix A_e_unscaled = inv_d_c_e.asDiagonal() * A_e;
  const DenseVector c_e_unscaled = inv_d_c_e.cwiseProduct(c_e);
  const SparseMatrix A_i_unscaled = inv_d_c_i.asDiagonal() * A_i;
  const DenseVector c_i_unscaled = inv_d_c_i.cwiseProduct(c_i);
  const DenseVector s_unscaled = inv_d_c_i.cwiseProduct(s);
  const DenseVector y_unscaled = scaling.c_e.cwiseProduct(y) * inv_d_f;
  const DenseVector z_unscaled = scaling.c_i.cwiseProduct(z) * inv_d_f;
  const Scalar μ_unscaled = inv_d_f * μ;

  return kkt_error<Scalar, T>(g_unscaled, A_e_unscaled, c_e_unscaled,
                              A_i_unscaled, c_i_unscaled, s_unscaled,
                              y_unscaled, z_unscaled, μ_unscaled);
}

}  // namespace slp
