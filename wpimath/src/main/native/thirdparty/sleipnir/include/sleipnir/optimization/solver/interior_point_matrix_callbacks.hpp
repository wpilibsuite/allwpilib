// Copyright (c) Sleipnir contributors

#pragma once

#include <functional>

#include <Eigen/Core>
#include <Eigen/SparseCore>

namespace slp {

/// Matrix callbacks for the interior-point method solver.
///
/// @tparam Scalar Scalar type.
template <typename Scalar>
struct InteriorPointMatrixCallbacks {
  /// Type alias for dense vector.
  using DenseVector = Eigen::Vector<Scalar, Eigen::Dynamic>;
  /// Type alias for sparse matrix.
  using SparseMatrix = Eigen::SparseMatrix<Scalar>;
  /// Type alias for sparse vector.
  using SparseVector = Eigen::SparseVector<Scalar>;

  /// Number of decision variables.
  int num_decision_variables = 0;

  /// Number of equality constraints.
  int num_equality_constraints = 0;

  /// Number of inequality constraints.
  int num_inequality_constraints = 0;

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
  std::function<Scalar(const DenseVector& x)> f;

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
  std::function<SparseVector(const DenseVector& x)> g;

  /// Lagrangian Hessian ∇ₓₓ²L(x, y, z) getter.
  ///
  /// L(x, y, z) = f(x) − yᵀcₑ(x) − zᵀcᵢ(x)
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
  ///     <td>z</td>
  ///     <td>num_inequality_constraints</td>
  ///     <td>1</td>
  ///   </tr>
  ///   <tr>
  ///     <td>∇ₓₓ²L(x, y, z)</td>
  ///     <td>num_decision_variables</td>
  ///     <td>num_decision_variables</td>
  ///   </tr>
  /// </table>
  std::function<SparseMatrix(const DenseVector& x, const DenseVector& y,
                             const DenseVector& z)>
      H;

  /// Constraint part of Lagrangian Hessian ∇ₓₓ²(−yᵀcₑ(x) − zᵀcᵢ(x)) getter.
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
  ///     <td>z</td>
  ///     <td>num_inequality_constraints</td>
  ///     <td>1</td>
  ///   </tr>
  ///   <tr>
  ///     <td>∇ₓₓ²(−yᵀcₑ(x) − zᵀcᵢ(x))</td>
  ///     <td>num_decision_variables</td>
  ///     <td>num_decision_variables</td>
  ///   </tr>
  /// </table>
  std::function<SparseMatrix(const DenseVector& x, const DenseVector& y,
                             const DenseVector& z)>
      H_c;

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
  std::function<DenseVector(const DenseVector& x)> c_e;

  /// Equality constraint Jacobian ∂cₑ/∂x getter.
  ///
  /// ```
  ///         [∇ᵀcₑ₁(x)]
  /// Aₑ(x) = [∇ᵀcₑ₂(x)]
  ///         [   ⋮    ]
  ///         [∇ᵀcₑₘ(x)]
  /// ```
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
  std::function<SparseMatrix(const DenseVector& x)> A_e;

  /// Inequality constraint value cᵢ(x) getter.
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
  ///     <td>cᵢ(x)</td>
  ///     <td>num_inequality_constraints</td>
  ///     <td>1</td>
  ///   </tr>
  /// </table>
  std::function<DenseVector(const DenseVector& x)> c_i;

  /// Inequality constraint Jacobian ∂cᵢ/∂x getter.
  ///
  /// ```
  ///         [∇ᵀcᵢ₁(x)]
  /// Aᵢ(x) = [∇ᵀcᵢ₂(x)]
  ///         [   ⋮    ]
  ///         [∇ᵀcᵢₘ(x)]
  /// ```
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
  ///     <td>Aᵢ(x)</td>
  ///     <td>num_inequality_constraints</td>
  ///     <td>num_decision_variables</td>
  ///   </tr>
  /// </table>
  std::function<SparseMatrix(const DenseVector& x)> A_i;
};

}  // namespace slp
