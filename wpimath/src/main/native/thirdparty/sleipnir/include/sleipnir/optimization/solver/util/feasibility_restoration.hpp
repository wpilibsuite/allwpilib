// Copyright (c) Sleipnir contributors

#pragma once

#include <algorithm>
#include <cmath>
#include <span>
#include <tuple>
#include <utility>

#include <Eigen/Core>
#include <gch/small_vector.hpp>

#include "sleipnir/optimization/solver/exit_status.hpp"
#include "sleipnir/optimization/solver/interior_point_matrix_callbacks.hpp"
#include "sleipnir/optimization/solver/iteration_info.hpp"
#include "sleipnir/optimization/solver/options.hpp"
#include "sleipnir/optimization/solver/sqp_matrix_callbacks.hpp"
#include "sleipnir/optimization/solver/util/append_as_triplets.hpp"
#include "sleipnir/optimization/solver/util/lagrange_multiplier_estimate.hpp"

namespace slp {

template <typename Scalar>
ExitStatus interior_point(
    const InteriorPointMatrixCallbacks<Scalar>& matrix_callbacks,
    std::span<std::function<bool(const IterationInfo<Scalar>& info)>>
        iteration_callbacks,
    const Options& options, bool in_feasibility_restoration,
#ifdef SLEIPNIR_ENABLE_BOUND_PROJECTION
    const Eigen::ArrayX<bool>& bound_constraint_mask,
#endif
    Eigen::Vector<Scalar, Eigen::Dynamic>& x,
    Eigen::Vector<Scalar, Eigen::Dynamic>& s,
    Eigen::Vector<Scalar, Eigen::Dynamic>& y,
    Eigen::Vector<Scalar, Eigen::Dynamic>& z, Scalar& μ);

/// Computes initial values for p and n in feasibility restoration.
///
/// @tparam Scalar Scalar type.
/// @param[in] c The constraint violation.
/// @param[in] ρ Scaling parameter.
/// @param[in] μ Barrier parameter.
/// @return Tuple of positive and negative constraint relaxation slack variables
///     respectively.
template <typename Scalar>
std::tuple<Eigen::Vector<Scalar, Eigen::Dynamic>,
           Eigen::Vector<Scalar, Eigen::Dynamic>>
compute_p_n(const Eigen::Vector<Scalar, Eigen::Dynamic>& c, Scalar ρ,
            Scalar μ) {
  // From equation (33) of [2]:
  //                       ______________________
  //       μ − ρ c(x)     /(μ − ρ c(x))²   μ c(x)
  //   n = −−−−−−−−−− +  / (−−−−−−−−−−)  + −−−−−−     (1)
  //           2ρ       √  (    2ρ    )      2ρ
  //
  // The quadratic formula:
  //             ________
  //       -b + √b² - 4ac
  //   x = −−−−−−−−−−−−−−                             (2)
  //             2a
  //
  // Rearrange (1) to fit the quadratic formula better:
  //                     _________________________
  //       μ - ρ c(x) + √(μ - ρ c(x))² + 2ρ μ c(x)
  //   n = −−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
  //                         2ρ
  //
  // Solve for coefficients:
  //
  //   a = ρ                                          (3)
  //   b = ρ c(x) - μ                                 (4)
  //
  //   -4ac = 2ρ μ c(x)
  //   -4(ρ)c = 2ρ μ c(x)
  //   -4c = 2μ c(x)
  //   c = -μ c(x)/2                                  (5)
  //
  //   p = c(x) + n                                   (6)

  using DenseVector = Eigen::Vector<Scalar, Eigen::Dynamic>;

  using std::sqrt;

  DenseVector p{c.rows()};
  DenseVector n{c.rows()};
  for (int row = 0; row < p.rows(); ++row) {
    Scalar _a = ρ;
    Scalar _b = ρ * c[row] - μ;
    Scalar _c = -μ * c[row] / Scalar(2);

    n[row] = (-_b + sqrt(_b * _b - Scalar(4) * _a * _c)) / (Scalar(2) * _a);
    p[row] = c[row] + n[row];
  }

  return {std::move(p), std::move(n)};
}

// @cond Suppress Doxygen

/// Finds the iterate that minimizes the constraint violation while not
/// deviating too far from the starting point. This is a fallback procedure when
/// the normal Sequential Quadratic Programming method fails to converge to a
/// feasible point.
///
/// @tparam Scalar Scalar type.
/// @param[in] matrix_callbacks Matrix callbacks.
/// @param[in] iteration_callbacks The list of callbacks to call at the
///     beginning of each iteration.
/// @param[in] options Solver options.
/// @param[in,out] x The decision variables from the normal solve.
/// @param[in,out] y The equality constraint dual variables from the normal
///     solve.
/// @return The exit status.
template <typename Scalar>
ExitStatus feasibility_restoration(
    const SQPMatrixCallbacks<Scalar>& matrix_callbacks,
    std::span<std::function<bool(const IterationInfo<Scalar>& info)>>
        iteration_callbacks,
    const Options& options, Eigen::Vector<Scalar, Eigen::Dynamic>& x,
    Eigen::Vector<Scalar, Eigen::Dynamic>& y) {
  // Feasibility restoration
  //
  //        min  ρ Σ (pₑ + nₑ) + ζ/2 (x - xᵣ)ᵀDᵣ(x - xᵣ)
  //         x
  //       pₑ,nₑ
  //
  //   s.t. cₑ(x) - pₑ + nₑ = 0
  //        pₑ ≥ 0
  //        nₑ ≥ 0
  //
  // where ρ = 1000, ζ = √μ where μ is the barrier parameter, xᵣ is original
  // iterate before feasibility restoration, and Dᵣ is a scaling matrix defined
  // by
  //
  //   Dᵣ = diag(min(1, 1/xᵣ[i]²) for i in x.rows())

  using DenseVector = Eigen::Vector<Scalar, Eigen::Dynamic>;
  using DiagonalMatrix = Eigen::DiagonalMatrix<Scalar, Eigen::Dynamic>;
  using SparseMatrix = Eigen::SparseMatrix<Scalar>;
  using SparseVector = Eigen::SparseVector<Scalar>;

  using std::sqrt;

  const auto& matrices = matrix_callbacks;
  const auto& num_vars = matrices.num_decision_variables;
  const auto& num_eq = matrices.num_equality_constraints;

  constexpr Scalar ρ(1e3);
  const Scalar μ(options.tolerance / 10.0);
  const Scalar ζ = sqrt(μ);

  const DenseVector c_e = matrices.c_e(x);

  const auto& x_r = x;
  const auto [p_e_0, n_e_0] = compute_p_n(c_e, ρ, μ);

  // Dᵣ = diag(min(1, 1/xᵣ[i]²) for i in x.rows())
  const DiagonalMatrix D_r =
      x.cwiseSquare().cwiseInverse().cwiseMin(Scalar(1)).asDiagonal();

  DenseVector fr_x{num_vars + 2 * num_eq};
  fr_x << x, p_e_0, n_e_0;

  DenseVector fr_s = DenseVector::Ones(2 * num_eq);

  DenseVector fr_y = DenseVector::Zero(num_eq);

  DenseVector fr_z{2 * num_eq};
  fr_z << μ * p_e_0.cwiseInverse(), μ * n_e_0.cwiseInverse();

  Scalar fr_μ = std::max(μ, c_e.template lpNorm<Eigen::Infinity>());

  InteriorPointMatrixCallbacks<Scalar> fr_matrix_callbacks{
      static_cast<int>(fr_x.rows()),
      static_cast<int>(fr_y.rows()),
      static_cast<int>(fr_z.rows()),
      [&](const DenseVector& x_p) -> Scalar {
        auto x = x_p.segment(0, num_vars);

        // Cost function
        //
        //   ρ Σ (pₑ + nₑ) + ζ/2 (x - xᵣ)ᵀDᵣ(x - xᵣ)

        auto diff = x - x_r;
        return ρ * x_p.segment(num_vars, 2 * num_eq).array().sum() +
               ζ / Scalar(2) * diff.transpose() * D_r * diff;
      },
      [&](const DenseVector& x_p) -> SparseVector {
        auto x = x_p.segment(0, num_vars);

        // Cost function gradient
        //
        //   [ζDᵣ(x − xᵣ)]
        //   [     ρ     ]
        //   [     ρ     ]
        DenseVector g{x_p.rows()};
        g.segment(0, num_vars) = ζ * D_r * (x - x_r);
        g.segment(num_vars, 2 * num_eq).setConstant(ρ);
        return g.sparseView();
      },
      [&](const DenseVector& x_p, const DenseVector& y_p,
          [[maybe_unused]] const DenseVector& z_p) -> SparseMatrix {
        auto x = x_p.segment(0, num_vars);
        const auto& y = y_p;

        // Cost function Hessian
        //
        //   [ζDᵣ  0  0]
        //   [ 0   0  0]
        //   [ 0   0  0]
        gch::small_vector<Eigen::Triplet<Scalar>> triplets;
        triplets.reserve(x_p.rows());
        append_as_triplets(triplets, 0, 0, {SparseMatrix{ζ * D_r}});
        SparseMatrix d2f_dx2{x_p.rows(), x_p.rows()};
        d2f_dx2.setFromSortedTriplets(triplets.begin(), triplets.end());

        // Constraint part of original problem's Lagrangian Hessian
        //
        //   −∇ₓₓ²yᵀcₑ(x)
        auto H_c = matrices.H_c(x, y);
        H_c.resize(x_p.rows(), x_p.rows());

        // Lagrangian Hessian
        //
        //   [ζDᵣ  0  0]
        //   [ 0   0  0] − ∇ₓₓ²yᵀcₑ(x)
        //   [ 0   0  0]
        return d2f_dx2 + H_c;
      },
      [&](const DenseVector& x_p, [[maybe_unused]] const DenseVector& y_p,
          [[maybe_unused]] const DenseVector& z_p) -> SparseMatrix {
        return SparseMatrix{x_p.rows(), x_p.rows()};
      },
      [&](const DenseVector& x_p) -> DenseVector {
        auto x = x_p.segment(0, num_vars);
        auto p_e = x_p.segment(num_vars, num_eq);
        auto n_e = x_p.segment(num_vars + num_eq, num_eq);

        // Equality constraints
        //
        //   cₑ(x) - pₑ + nₑ = 0
        return matrices.c_e(x) - p_e + n_e;
      },
      [&](const DenseVector& x_p) -> SparseMatrix {
        auto x = x_p.segment(0, num_vars);

        // Equality constraint Jacobian
        //
        //   [Aₑ  −I  I]

        SparseMatrix A_e = matrices.A_e(x);

        gch::small_vector<Eigen::Triplet<Scalar>> triplets;
        triplets.reserve(A_e.nonZeros() + 2 * num_eq);

        append_as_triplets(triplets, 0, 0, {A_e});
        append_diagonal_as_triplets(
            triplets, 0, num_vars,
            DenseVector::Constant(num_eq, Scalar(-1)).eval());
        append_diagonal_as_triplets(
            triplets, 0, num_vars + num_eq,
            DenseVector::Constant(num_eq, Scalar(1)).eval());

        SparseMatrix A_e_p{A_e.rows(), x_p.rows()};
        A_e_p.setFromSortedTriplets(triplets.begin(), triplets.end());
        return A_e_p;
      },
      [&](const DenseVector& x_p) -> DenseVector {
        // Inequality constraints
        //
        //   pₑ ≥ 0
        //   nₑ ≥ 0
        return x_p.segment(num_vars, 2 * num_eq);
      },
      [&](const DenseVector& x_p) -> SparseMatrix {
        // Inequality constraint Jacobian
        //
        //   [0  I  0]
        //   [0  0  I]

        gch::small_vector<Eigen::Triplet<Scalar>> triplets;
        triplets.reserve(2 * num_eq);

        append_diagonal_as_triplets(
            triplets, 0, num_vars,
            DenseVector::Constant(2 * num_eq, Scalar(1)).eval());

        SparseMatrix A_i_p{2 * num_eq, x_p.rows()};
        A_i_p.setFromSortedTriplets(triplets.begin(), triplets.end());
        return A_i_p;
      }};

  auto status = interior_point<Scalar>(fr_matrix_callbacks, iteration_callbacks,
                                       options, true,
#ifdef SLEIPNIR_ENABLE_BOUND_PROJECTION
                                       {},
#endif
                                       fr_x, fr_s, fr_y, fr_z, fr_μ);

  x = fr_x.segment(0, x.rows());

  if (status == ExitStatus::CALLBACK_REQUESTED_STOP) {
    auto g = matrices.g(x);
    auto A_e = matrices.A_e(x);

    y = lagrange_multiplier_estimate(g, A_e);

    return ExitStatus::SUCCESS;
  } else if (status == ExitStatus::SUCCESS) {
    return ExitStatus::LOCALLY_INFEASIBLE;
  } else {
    return ExitStatus::FEASIBILITY_RESTORATION_FAILED;
  }
}

/// Finds the iterate that minimizes the constraint violation while not
/// deviating too far from the starting point. This is a fallback procedure when
/// the normal interior-point method fails to converge to a feasible point.
///
/// @tparam Scalar Scalar type.
/// @param[in] matrix_callbacks Matrix callbacks.
/// @param[in] iteration_callbacks The list of callbacks to call at the
///     beginning of each iteration.
/// @param[in] options Solver options.
/// @param[in,out] x The current decision variables from the normal solve.
/// @param[in,out] s The current inequality constraint slack variables from the
///     normal solve.
/// @param[in,out] y The current equality constraint duals from the normal
///     solve.
/// @param[in,out] z The current inequality constraint duals from the normal
///     solve.
/// @param[in] μ Barrier parameter.
/// @return The exit status.
template <typename Scalar>
ExitStatus feasibility_restoration(
    const InteriorPointMatrixCallbacks<Scalar>& matrix_callbacks,
    std::span<std::function<bool(const IterationInfo<Scalar>& info)>>
        iteration_callbacks,
    const Options& options, Eigen::Vector<Scalar, Eigen::Dynamic>& x,
    Eigen::Vector<Scalar, Eigen::Dynamic>& s,
    Eigen::Vector<Scalar, Eigen::Dynamic>& y,
    Eigen::Vector<Scalar, Eigen::Dynamic>& z, Scalar μ) {
  // Feasibility restoration
  //
  //        min  ρ Σ (pₑ + nₑ + pᵢ + nᵢ) + ζ/2 (x - xᵣ)ᵀDᵣ(x - xᵣ)
  //         x
  //       pₑ,nₑ
  //       pᵢ,nᵢ
  //
  //   s.t. cₑ(x) - pₑ + nₑ = 0
  //        cᵢ(x) - pᵢ + nᵢ ≥ 0
  //        pₑ ≥ 0
  //        nₑ ≥ 0
  //        pᵢ ≥ 0
  //        nᵢ ≥ 0
  //
  // where ρ = 1000, ζ = √μ where μ is the barrier parameter, xᵣ is original
  // iterate before feasibility restoration, and Dᵣ is a scaling matrix defined
  // by
  //
  //   Dᵣ = diag(min(1, 1/xᵣ[i]²) for i in x.rows())

  using DenseVector = Eigen::Vector<Scalar, Eigen::Dynamic>;
  using DiagonalMatrix = Eigen::DiagonalMatrix<Scalar, Eigen::Dynamic>;
  using SparseMatrix = Eigen::SparseMatrix<Scalar>;
  using SparseVector = Eigen::SparseVector<Scalar>;

  using std::sqrt;

  const auto& matrices = matrix_callbacks;
  const auto& num_vars = matrices.num_decision_variables;
  const auto& num_eq = matrices.num_equality_constraints;
  const auto& num_ineq = matrices.num_inequality_constraints;

  constexpr Scalar ρ(1e3);
  const Scalar ζ = sqrt(μ);

  const DenseVector c_e = matrices.c_e(x);
  const DenseVector c_i = matrices.c_i(x);

  const auto& x_r = x;
  const auto [p_e_0, n_e_0] = compute_p_n(c_e, ρ, μ);
  const auto [p_i_0, n_i_0] = compute_p_n((c_i - s).eval(), ρ, μ);

  // Dᵣ = diag(min(1, 1/xᵣ[i]²) for i in x.rows())
  const DiagonalMatrix D_r =
      x.cwiseSquare().cwiseInverse().cwiseMin(Scalar(1)).asDiagonal();

  DenseVector fr_x{num_vars + 2 * num_eq + 2 * num_ineq};
  fr_x << x, p_e_0, n_e_0, p_i_0, n_i_0;

  DenseVector fr_s{s.rows() + 2 * num_eq + 2 * num_ineq};
  fr_s.segment(0, s.rows()) = s;
  fr_s.segment(s.rows(), 2 * num_eq + 2 * num_ineq).setOnes();

  DenseVector fr_y = DenseVector::Zero(c_e.rows());

  DenseVector fr_z{c_i.rows() + 2 * num_eq + 2 * num_ineq};
  fr_z << z.cwiseMin(ρ), μ * p_e_0.cwiseInverse(), μ * n_e_0.cwiseInverse(),
      μ * p_i_0.cwiseInverse(), μ * n_i_0.cwiseInverse();

  Scalar fr_μ = std::max({μ, c_e.template lpNorm<Eigen::Infinity>(),
                          (c_i - s).template lpNorm<Eigen::Infinity>()});

  InteriorPointMatrixCallbacks<Scalar> fr_matrix_callbacks{
      static_cast<int>(fr_x.rows()),
      static_cast<int>(fr_y.rows()),
      static_cast<int>(fr_z.rows()),
      [&](const DenseVector& x_p) -> Scalar {
        auto x = x_p.segment(0, num_vars);

        // Cost function
        //
        //   ρ Σ (pₑ + nₑ + pᵢ + nᵢ) + ζ/2 (x - xᵣ)ᵀDᵣ(x - xᵣ)
        auto diff = x - x_r;
        return ρ * x_p.segment(num_vars, 2 * num_eq + 2 * num_ineq)
                       .array()
                       .sum() +
               ζ / Scalar(2) * diff.transpose() * D_r * diff;
      },
      [&](const DenseVector& x_p) -> SparseVector {
        auto x = x_p.segment(0, num_vars);

        // Cost function gradient
        //
        //   [ζDᵣ(x − xᵣ)]
        //   [     ρ     ]
        //   [     ρ     ]
        //   [     ρ     ]
        //   [     ρ     ]
        DenseVector g{x_p.rows()};
        g.segment(0, num_vars) = ζ * D_r * (x - x_r);
        g.segment(num_vars, 2 * num_eq + 2 * num_ineq).setConstant(ρ);
        return g.sparseView();
      },
      [&](const DenseVector& x_p, const DenseVector& y_p,
          const DenseVector& z_p) -> SparseMatrix {
        auto x = x_p.segment(0, num_vars);
        const auto& y = y_p;
        auto z = z_p.segment(0, num_ineq);

        // Cost function Hessian
        //
        //   [ζDᵣ  0  0  0  0]
        //   [ 0   0  0  0  0]
        //   [ 0   0  0  0  0]
        //   [ 0   0  0  0  0]
        //   [ 0   0  0  0  0]
        gch::small_vector<Eigen::Triplet<Scalar>> triplets;
        triplets.reserve(x_p.rows());
        append_as_triplets(triplets, 0, 0, {SparseMatrix{ζ * D_r}});
        SparseMatrix d2f_dx2{x_p.rows(), x_p.rows()};
        d2f_dx2.setFromSortedTriplets(triplets.begin(), triplets.end());

        // Constraint part of original problem's Lagrangian Hessian
        //
        //   −∇ₓₓ²yᵀcₑ(x) − ∇ₓₓ²zᵀcᵢ(x)
        auto H_c = matrices.H_c(x, y, z);
        H_c.resize(x_p.rows(), x_p.rows());

        // Lagrangian Hessian
        //
        //   [ζDᵣ  0  0  0  0]
        //   [ 0   0  0  0  0]
        //   [ 0   0  0  0  0] − ∇ₓₓ²yᵀcₑ(x) − ∇ₓₓ²zᵀcᵢ(x)
        //   [ 0   0  0  0  0]
        //   [ 0   0  0  0  0]
        return d2f_dx2 + H_c;
      },
      [&](const DenseVector& x_p, [[maybe_unused]] const DenseVector& y_p,
          [[maybe_unused]] const DenseVector& z_p) -> SparseMatrix {
        return SparseMatrix{x_p.rows(), x_p.rows()};
      },
      [&](const DenseVector& x_p) -> DenseVector {
        auto x = x_p.segment(0, num_vars);
        auto p_e = x_p.segment(num_vars, num_eq);
        auto n_e = x_p.segment(num_vars + num_eq, num_eq);

        // Equality constraints
        //
        //   cₑ(x) - pₑ + nₑ = 0
        return matrices.c_e(x) - p_e + n_e;
      },
      [&](const DenseVector& x_p) -> SparseMatrix {
        auto x = x_p.segment(0, num_vars);

        // Equality constraint Jacobian
        //
        //   [Aₑ  −I  I  0  0]

        SparseMatrix A_e = matrices.A_e(x);

        gch::small_vector<Eigen::Triplet<Scalar>> triplets;
        triplets.reserve(A_e.nonZeros() + 2 * num_eq);

        append_as_triplets(triplets, 0, 0, {A_e});
        append_diagonal_as_triplets(
            triplets, 0, num_vars,
            DenseVector::Constant(num_eq, Scalar(-1)).eval());
        append_diagonal_as_triplets(
            triplets, 0, num_vars + num_eq,
            DenseVector::Constant(num_eq, Scalar(1)).eval());

        SparseMatrix A_e_p{A_e.rows(), x_p.rows()};
        A_e_p.setFromSortedTriplets(triplets.begin(), triplets.end());
        return A_e_p;
      },
      [&](const DenseVector& x_p) -> DenseVector {
        auto x = x_p.segment(0, num_vars);
        auto p_i = x_p.segment(num_vars + 2 * num_eq, num_ineq);
        auto n_i = x_p.segment(num_vars + 2 * num_eq + num_ineq, num_ineq);

        // Inequality constraints
        //
        //   cᵢ(x) - pᵢ + nᵢ ≥ 0
        //   pₑ ≥ 0
        //   nₑ ≥ 0
        //   pᵢ ≥ 0
        //   nᵢ ≥ 0
        DenseVector c_i_p{c_i.rows() + 2 * num_eq + 2 * num_ineq};
        c_i_p.segment(0, num_ineq) = matrices.c_i(x) - p_i + n_i;
        c_i_p.segment(p_i.rows(), 2 * num_eq + 2 * num_ineq) =
            x_p.segment(num_vars, 2 * num_eq + 2 * num_ineq);
        return c_i_p;
      },
      [&](const DenseVector& x_p) -> SparseMatrix {
        auto x = x_p.segment(0, num_vars);

        // Inequality constraint Jacobian
        //
        //   [Aᵢ  0  0  −I  I]
        //   [0   I  0   0  0]
        //   [0   0  I   0  0]
        //   [0   0  0   I  0]
        //   [0   0  0   0  I]

        SparseMatrix A_i = matrices.A_i(x);

        gch::small_vector<Eigen::Triplet<Scalar>> triplets;
        triplets.reserve(A_i.nonZeros() + 2 * num_eq + 4 * num_ineq);

        // Column 0
        append_as_triplets(triplets, 0, 0, {A_i});

        // Columns 1 and 2
        append_diagonal_as_triplets(
            triplets, num_ineq, num_vars,
            DenseVector::Constant(2 * num_eq, Scalar(1)).eval());

        SparseMatrix I_ineq{
            DenseVector::Constant(num_ineq, Scalar(1)).asDiagonal()};

        // Column 3
        SparseMatrix Z_col3{2 * num_eq, num_ineq};
        append_as_triplets(triplets, 0, num_vars + 2 * num_eq,
                           {(-I_ineq).eval(), Z_col3, I_ineq});

        // Column 4
        SparseMatrix Z_col4{2 * num_eq + num_ineq, num_ineq};
        append_as_triplets(triplets, 0, num_vars + 2 * num_eq + num_ineq,
                           {I_ineq, Z_col4, I_ineq});

        SparseMatrix A_i_p{2 * num_eq + 3 * num_ineq, x_p.rows()};
        A_i_p.setFromSortedTriplets(triplets.begin(), triplets.end());
        return A_i_p;
      }};

  auto status = interior_point<Scalar>(fr_matrix_callbacks, iteration_callbacks,
                                       options, true,
#ifdef SLEIPNIR_ENABLE_BOUND_PROJECTION
                                       {},
#endif
                                       fr_x, fr_s, fr_y, fr_z, fr_μ);

  x = fr_x.segment(0, x.rows());
  s = fr_s.segment(0, s.rows());

  if (status == ExitStatus::CALLBACK_REQUESTED_STOP) {
    auto g = matrices.g(x);
    auto A_e = matrices.A_e(x);
    auto A_i = matrices.A_i(x);

    auto [y_estimate, z_estimate] =
        lagrange_multiplier_estimate(g, A_e, A_i, s, μ);
    y = y_estimate;
    z = z_estimate;

    return ExitStatus::SUCCESS;
  } else if (status == ExitStatus::SUCCESS) {
    return ExitStatus::LOCALLY_INFEASIBLE;
  } else {
    return ExitStatus::FEASIBILITY_RESTORATION_FAILED;
  }
}

// @endcond

}  // namespace slp

#include "sleipnir/optimization/solver/interior_point.hpp"
