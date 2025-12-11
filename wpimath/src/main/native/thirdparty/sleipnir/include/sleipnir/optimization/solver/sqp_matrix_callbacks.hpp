// Copyright (c) Sleipnir contributors

#pragma once

#include <functional>

#include <Eigen/Core>
#include <Eigen/SparseCore>

namespace slp {

/**
 * Matrix callbacks for the Sequential Quadratic Programming (SQP) solver.
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct SQPMatrixCallbacks {
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
  std::function<Scalar(const Eigen::Vector<Scalar, Eigen::Dynamic>& x)> f;

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
  std::function<Eigen::SparseVector<Scalar>(
      const Eigen::Vector<Scalar, Eigen::Dynamic>& x)>
      g;

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
  std::function<Eigen::SparseMatrix<Scalar>(
      const Eigen::Vector<Scalar, Eigen::Dynamic>& x,
      const Eigen::Vector<Scalar, Eigen::Dynamic>& y)>
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
  std::function<Eigen::Vector<Scalar, Eigen::Dynamic>(
      const Eigen::Vector<Scalar, Eigen::Dynamic>& x)>
      c_e;

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
  std::function<Eigen::SparseMatrix<Scalar>(
      const Eigen::Vector<Scalar, Eigen::Dynamic>& x)>
      A_e;
};

}  // namespace slp
