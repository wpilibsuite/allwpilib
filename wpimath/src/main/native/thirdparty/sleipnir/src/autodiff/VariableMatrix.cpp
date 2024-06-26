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

  if (A.Rows() == 1 && A.Cols() == 1) {
    // Compute optimal inverse instead of using Eigen's general solver
    return B(0, 0) / A(0, 0);
  } else if (A.Rows() == 2 && A.Cols() == 2) {
    // Compute optimal inverse instead of using Eigen's general solver
    //
    // [a  b]⁻¹  ___1___ [ d  −b]
    // [c  d]  = ad − bc [−c   a]

    const auto& a = A(0, 0);
    const auto& b = A(0, 1);
    const auto& c = A(1, 0);
    const auto& d = A(1, 1);

    sleipnir::VariableMatrix Ainv{{d, -b}, {-c, a}};
    auto detA = a * d - b * c;
    Ainv /= detA;

    return Ainv * B;
  } else if (A.Rows() == 3 && A.Cols() == 3) {
    // Compute optimal inverse instead of using Eigen's general solver
    //
    // [a  b  c]⁻¹
    // [d  e  f]
    // [g  h  i]
    //                       1                 [ei − fh  ch − bi  bf − ce]
    //     = --------------------------------- [fg − di  ai − cg  cd − af]
    //       aei − afh − bdi + bfg + cdh − ceg [dh − eg  bg − ah  ae − bd]

    const auto& a = A(0, 0);
    const auto& b = A(0, 1);
    const auto& c = A(0, 2);
    const auto& d = A(1, 0);
    const auto& e = A(1, 1);
    const auto& f = A(1, 2);
    const auto& g = A(2, 0);
    const auto& h = A(2, 1);
    const auto& i = A(2, 2);

    sleipnir::VariableMatrix Ainv{
        {e * i - f * h, c * h - b * i, b * f - c * e},
        {f * g - d * i, a * i - c * g, c * d - a * f},
        {d * h - e * g, b * g - a * h, a * e - b * d}};
    auto detA =
        a * e * i - a * f * h - b * d * i + b * f * g + c * d * h - c * e * g;
    Ainv /= detA;

    return Ainv * B;
  } else {
    using MatrixXv = Eigen::Matrix<Variable, Eigen::Dynamic, Eigen::Dynamic>;

    MatrixXv eigenA{A.Rows(), A.Cols()};
    for (int row = 0; row < A.Rows(); ++row) {
      for (int col = 0; col < A.Cols(); ++col) {
        eigenA(row, col) = A(row, col);
      }
    }

    MatrixXv eigenB{B.Rows(), B.Cols()};
    for (int row = 0; row < B.Rows(); ++row) {
      for (int col = 0; col < B.Cols(); ++col) {
        eigenB(row, col) = B(row, col);
      }
    }

    MatrixXv eigenX = eigenA.householderQr().solve(eigenB);

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
