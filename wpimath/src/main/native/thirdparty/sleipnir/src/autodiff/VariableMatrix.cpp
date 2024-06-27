// Copyright (c) Sleipnir contributors

#include "sleipnir/autodiff/VariableMatrix.hpp"

#include <Eigen/QR>

namespace Eigen {

template <>
struct NumTraits<sleipnir::Variable> : NumTraits<double> {
  using Real = sleipnir::Variable;
  using NonInteger = sleipnir::Variable;
  using Nested = sleipnir::Variable;

  enum {
    IsComplex = 0,
    IsInteger = 0,
    IsSigned = 1,
    RequireInitialization = 1,
    ReadCost = 1,
    AddCost = 3,
    MulCost = 3
  };
};

}  // namespace Eigen

// For Variable equality operator
#include "sleipnir/optimization/Constraints.hpp"

namespace sleipnir {

VariableMatrix Solve(const VariableMatrix& A, const VariableMatrix& B) {
  // m x n * n x p = m x p
  Assert(A.Rows() == B.Rows());

  if (A.Rows() == 2 && A.Cols() == 2) {
    // Compute optimal inverse instead of using Eigen's general solver
    sleipnir::VariableMatrix Ainv{2, 2};
    Ainv(0, 0) = A(1, 1);
    Ainv(0, 1) = -A(0, 1);
    Ainv(1, 0) = -A(1, 0);
    Ainv(1, 1) = A(0, 0);
    auto detA = A(0, 0) * A(1, 1) - A(0, 1) * A(1, 0);
    Ainv /= detA;

    return Ainv * B;
  } else {
    using MatrixXvar = Eigen::Matrix<Variable, Eigen::Dynamic, Eigen::Dynamic>;

    MatrixXvar eigenA{A.Rows(), A.Cols()};
    for (int row = 0; row < A.Rows(); ++row) {
      for (int col = 0; col < A.Cols(); ++col) {
        eigenA(row, col) = A(row, col);
      }
    }

    MatrixXvar eigenB{B.Rows(), B.Cols()};
    for (int row = 0; row < B.Rows(); ++row) {
      for (int col = 0; col < B.Cols(); ++col) {
        eigenB(row, col) = B(row, col);
      }
    }

    MatrixXvar eigenX = eigenA.householderQr().solve(eigenB);

    VariableMatrix X{A.Cols(), B.Cols()};
    for (int row = 0; row < X.Rows(); ++row) {
      for (int col = 0; col < X.Cols(); ++col) {
        X(row, col) = eigenX(row, col);
      }
    }

    return X;
  }
}

}  // namespace sleipnir
