// Copyright (c) Sleipnir contributors

#pragma once

#include <functional>
#include <span>

#include <Eigen/Core>
#include <Eigen/SparseCore>

#include "sleipnir/optimization/solver/exit_status.hpp"
#include "sleipnir/optimization/solver/iteration_info.hpp"
#include "sleipnir/optimization/solver/options.hpp"
#include "sleipnir/util/symbol_exports.hpp"

namespace slp {

/**
 * Matrix callbacks for the Sequential Quadratic Programming (SQP) solver.
 */
struct SLEIPNIR_DLLEXPORT SQPMatrixCallbacks {
  /// Cost function value f(x) getter.
  ///
  /// <table>
  ///   <tr>
  ///     <th>Variable</th>
  ///     <th>Rows</th>
  ///     <th>Columns</th>
  ///   </tr>
  ///   <tr>
  ///     <td>x</td>
  ///     <td>num_decision_variables</td>
  ///     <td>1</td>
  ///   </tr>
  ///   <tr>
  ///     <td>f(x)</td>
  ///     <td>1</td>
  ///     <td>1</td>
  ///   </tr>
  /// </table>
  std::function<double(const Eigen::VectorXd& x)> f;

  /// Cost function gradient ∇f(x) getter.
  ///
  /// <table>
  ///   <tr>
  ///     <th>Variable</th>
  ///     <th>Rows</th>
  ///     <th>Columns</th>
  ///   </tr>
  ///   <tr>
  ///     <td>x</td>
  ///     <td>num_decision_variables</td>
  ///     <td>1</td>
  ///   </tr>
  ///   <tr>
  ///     <td>∇f(x)</td>
  ///     <td>num_decision_variables</td>
  ///     <td>1</td>
  ///   </tr>
  /// </table>
  std::function<Eigen::SparseVector<double>(const Eigen::VectorXd& x)> g;

  /// Lagrangian Hessian ∇ₓₓ²L(x, y) getter.
  ///
  /// L(xₖ, yₖ) = f(xₖ) − yₖᵀcₑ(xₖ)
  ///
  /// <table>
  ///   <tr>
  ///     <th>Variable</th>
  ///     <th>Rows</th>
  ///     <th>Columns</th>
  ///   </tr>
  ///   <tr>
  ///     <td>x</td>
  ///     <td>num_decision_variables</td>
  ///     <td>1</td>
  ///   </tr>
  ///   <tr>
  ///     <td>y</td>
  ///     <td>num_equality_constraints</td>
  ///     <td>1</td>
  ///   </tr>
  ///   <tr>
  ///     <td>∇ₓₓ²L(x, y)</td>
  ///     <td>num_decision_variables</td>
  ///     <td>num_decision_variables</td>
  ///   </tr>
  /// </table>
  std::function<Eigen::SparseMatrix<double>(const Eigen::VectorXd& x,
                                            const Eigen::VectorXd& y)>
      H;

  /// Equality constraint value cₑ(x) getter.
  ///
  /// <table>
  ///   <tr>
  ///     <th>Variable</th>
  ///     <th>Rows</th>
  ///     <th>Columns</th>
  ///   </tr>
  ///   <tr>
  ///     <td>x</td>
  ///     <td>num_decision_variables</td>
  ///     <td>1</td>
  ///   </tr>
  ///   <tr>
  ///     <td>cₑ(x)</td>
  ///     <td>num_equality_constraints</td>
  ///     <td>1</td>
  ///   </tr>
  /// </table>
  std::function<Eigen::VectorXd(const Eigen::VectorXd& x)> c_e;

  /// Equality constraint Jacobian ∂cₑ/∂x getter.
  ///
  /// @verbatim
  ///         [∇ᵀcₑ₁(xₖ)]
  /// Aₑ(x) = [∇ᵀcₑ₂(xₖ)]
  ///         [    ⋮    ]
  ///         [∇ᵀcₑₘ(xₖ)]
  /// @endverbatim
  ///
  /// <table>
  ///   <tr>
  ///     <th>Variable</th>
  ///     <th>Rows</th>
  ///     <th>Columns</th>
  ///   </tr>
  ///   <tr>
  ///     <td>x</td>
  ///     <td>num_decision_variables</td>
  ///     <td>1</td>
  ///   </tr>
  ///   <tr>
  ///     <td>Aₑ(x)</td>
  ///     <td>num_equality_constraints</td>
  ///     <td>num_decision_variables</td>
  ///   </tr>
  /// </table>
  std::function<Eigen::SparseMatrix<double>(const Eigen::VectorXd& x)> A_e;
};

/**
Finds the optimal solution to a nonlinear program using Sequential Quadratic
Programming (SQP).

A nonlinear program has the form:

@verbatim
     min_x f(x)
subject to cₑ(x) = 0
@endverbatim

where f(x) is the cost function and cₑ(x) are the equality constraints.

@param[in] matrix_callbacks Matrix callbacks.
@param[in] iteration_callbacks The list of callbacks to call at the beginning of
  each iteration.
@param[in] options Solver options.
@param[in,out] x The initial guess and output location for the decision
  variables.
@return The exit status.
*/
SLEIPNIR_DLLEXPORT ExitStatus
sqp(const SQPMatrixCallbacks& matrix_callbacks,
    std::span<std::function<bool(const IterationInfo& info)>>
        iteration_callbacks,
    const Options& options, Eigen::VectorXd& x);

}  // namespace slp
