// Copyright (c) Sleipnir contributors

#pragma once

#include <algorithm>

#include <Eigen/Core>
#include <Eigen/SparseCore>

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
/// @tparam T The type of KKT error to compute.
/// @param g Gradient of the cost function ∇f.
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
/// @tparam T The type of KKT error to compute.
/// @param g Gradient of the cost function ∇f.
/// @param A_e The problem's equality constraint Jacobian Aₑ(x) evaluated at the
///     current iterate.
/// @param c_e The problem's equality constraints cₑ(x) evaluated at the current
///     iterate.
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
/// @tparam T The type of KKT error to compute.
/// @param g Gradient of the cost function ∇f.
/// @param A_e The problem's equality constraint Jacobian Aₑ(x) evaluated at the
///     current iterate.
/// @param c_e The problem's equality constraints cₑ(x) evaluated at the current
///     iterate.
/// @param A_i The problem's inequality constraint Jacobian Aᵢ(x) evaluated at
///     the current iterate.
/// @param c_i The problem's inequality constraints cᵢ(x) evaluated at the
///     current iterate.
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

}  // namespace slp
