// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <fmt/core.h>

#include "Eigen/Core"
#include "Eigen/Eigenvalues"
#include "frc/DARE.h"
#include "frc/fmt/Eigen.h"
#include "gtest/gtest.h"

void ExpectMatrixEqual(const Eigen::MatrixXd& lhs, const Eigen::MatrixXd& rhs,
                       double tolerance) {
  for (int row = 0; row < lhs.rows(); ++row) {
    for (int col = 0; col < lhs.cols(); ++col) {
      EXPECT_NEAR(lhs(row, col), rhs(row, col), tolerance)
          << fmt::format("row = {}, col = {}", row, col);
    }
  }

  if (::testing::Test::HasFailure()) {
    fmt::print("lhs =\n{}\n", lhs);
    fmt::print("rhs =\n{}\n", rhs);
    fmt::print("delta =\n{}\n", Eigen::MatrixXd{lhs - rhs});
  }
}

void ExpectPositiveSemidefinite(const Eigen::Ref<const Eigen::MatrixXd>& X) {
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigX{X,
                                                      Eigen::EigenvaluesOnly};
  for (int i = 0; i < X.rows(); ++i) {
    EXPECT_GE(eigX.eigenvalues()[i], 0.0);
  }
}

void ExpectDARESolution(const Eigen::Ref<const Eigen::MatrixXd>& A,
                        const Eigen::Ref<const Eigen::MatrixXd>& B,
                        const Eigen::Ref<const Eigen::MatrixXd>& Q,
                        const Eigen::Ref<const Eigen::MatrixXd>& R,
                        const Eigen::Ref<const Eigen::MatrixXd>& X) {
  // Check that X is the solution to the DARE
  // Y = AᵀXA − X − AᵀXB(BᵀXB + R)⁻¹BᵀXA + Q = 0
  // clang-format off
  Eigen::MatrixXd Y =
      A.transpose() * X * A
      - X
      - (A.transpose() * X * B * (B.transpose() * X * B + R).inverse()
        * B.transpose() * X * A)
      + Q;
  // clang-format on
  ExpectMatrixEqual(Y, Eigen::MatrixXd::Zero(X.rows(), X.cols()), 1e-10);
}

void ExpectDARESolution(const Eigen::Ref<const Eigen::MatrixXd>& A,
                        const Eigen::Ref<const Eigen::MatrixXd>& B,
                        const Eigen::Ref<const Eigen::MatrixXd>& Q,
                        const Eigen::Ref<const Eigen::MatrixXd>& R,
                        const Eigen::Ref<const Eigen::MatrixXd>& N,
                        const Eigen::Ref<const Eigen::MatrixXd>& X) {
  // Check that X is the solution to the DARE
  // Y = AᵀXA − X − (AᵀXB + N)(BᵀXB + R)⁻¹(BᵀXA + Nᵀ) + Q = 0
  // clang-format off
  Eigen::MatrixXd Y =
      A.transpose() * X * A
      - X
      - ((A.transpose() * X * B + N) * (B.transpose() * X * B + R).inverse()
        * (B.transpose() * X * A + N.transpose()))
      + Q;
  // clang-format on
  ExpectMatrixEqual(Y, Eigen::MatrixXd::Zero(X.rows(), X.cols()), 1e-10);
}

TEST(DARETest, NonInvertibleA_ABQR) {
  // Example 2 of "On the Numerical Solution of the Discrete-Time Algebraic
  // Riccati Equation"

  Eigen::MatrixXd A{4, 4};
  A << 0.5, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0;
  Eigen::MatrixXd B{4, 1};
  B << 0, 0, 0, 1;
  Eigen::MatrixXd Q{4, 4};
  Q << 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
  Eigen::MatrixXd R{1, 1};
  R << 0.25;

  Eigen::MatrixXd X = frc::DARE(A, B, Q, R);
  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, X);
}

TEST(DARETest, NonInvertibleA_ABQRN) {
  // Example 2 of "On the Numerical Solution of the Discrete-Time Algebraic
  // Riccati Equation"

  Eigen::MatrixXd A{4, 4};
  A << 0.5, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0;
  Eigen::MatrixXd B{4, 1};
  B << 0, 0, 0, 1;
  Eigen::MatrixXd Q{4, 4};
  Q << 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
  Eigen::MatrixXd R{1, 1};
  R << 0.25;

  Eigen::MatrixXd Aref{4, 4};
  Aref << 0.25, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0;
  Q = (A - Aref).transpose() * Q * (A - Aref);
  R = B.transpose() * Q * B + R;
  Eigen::MatrixXd N = (A - Aref).transpose() * Q * B;

  Eigen::MatrixXd X = frc::DARE(A, B, Q, R, N);
  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, N, X);
}

TEST(DARETest, InvertibleA_ABQR) {
  Eigen::MatrixXd A{2, 2};
  A << 1, 1, 0, 1;
  Eigen::MatrixXd B{2, 1};
  B << 0, 1;
  Eigen::MatrixXd Q{2, 2};
  Q << 1, 0, 0, 0;
  Eigen::MatrixXd R{1, 1};
  R << 0.3;

  Eigen::MatrixXd X = frc::DARE(A, B, Q, R);
  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, X);
}

TEST(DARETest, InvertibleA_ABQRN) {
  Eigen::MatrixXd A{2, 2};
  A << 1, 1, 0, 1;
  Eigen::MatrixXd B{2, 1};
  B << 0, 1;
  Eigen::MatrixXd Q{2, 2};
  Q << 1, 0, 0, 0;
  Eigen::MatrixXd R{1, 1};
  R << 0.3;

  Eigen::MatrixXd Aref{2, 2};
  Aref << 0.5, 1, 0, 1;
  Q = (A - Aref).transpose() * Q * (A - Aref);
  R = B.transpose() * Q * B + R;
  Eigen::MatrixXd N = (A - Aref).transpose() * Q * B;

  Eigen::MatrixXd X = frc::DARE(A, B, Q, R, N);
  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, N, X);
}

TEST(DARETest, FirstGeneralizedEigenvalueOfSTIsStable_ABQR) {
  // The first generalized eigenvalue of (S, T) is stable

  Eigen::MatrixXd A{2, 2};
  A << 0, 1, 0, 0;
  Eigen::MatrixXd B{2, 1};
  B << 0, 1;
  Eigen::MatrixXd Q{2, 2};
  Q << 1, 0, 0, 1;
  Eigen::MatrixXd R{1, 1};
  R << 1;

  Eigen::MatrixXd X = frc::DARE(A, B, Q, R);
  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, X);
}

TEST(DARETest, FirstGeneralizedEigenvalueOfSTIsStable_ABQRN) {
  // The first generalized eigenvalue of (S, T) is stable

  Eigen::MatrixXd A{2, 2};
  A << 0, 1, 0, 0;
  Eigen::MatrixXd B{2, 1};
  B << 0, 1;
  Eigen::MatrixXd Q{2, 2};
  Q << 1, 0, 0, 1;
  Eigen::MatrixXd R{1, 1};
  R << 1;

  Eigen::MatrixXd Aref{2, 2};
  Aref << 0, 0.5, 0, 0;
  Q = (A - Aref).transpose() * Q * (A - Aref);
  R = B.transpose() * Q * B + R;
  Eigen::MatrixXd N = (A - Aref).transpose() * Q * B;

  Eigen::MatrixXd X = frc::DARE(A, B, Q, R, N);
  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, N, X);
}

TEST(DARETest, IdentitySystem_ABQR) {
  const Eigen::MatrixXd A{Eigen::Matrix2d::Identity()};
  const Eigen::MatrixXd B{Eigen::Matrix2d::Identity()};
  const Eigen::MatrixXd Q{Eigen::Matrix2d::Identity()};
  const Eigen::MatrixXd R{Eigen::Matrix2d::Identity()};

  Eigen::MatrixXd X = frc::DARE(A, B, Q, R);
  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, X);
}

TEST(DARETest, IdentitySystem_ABQRN) {
  const Eigen::MatrixXd A{Eigen::Matrix2d::Identity()};
  const Eigen::MatrixXd B{Eigen::Matrix2d::Identity()};
  const Eigen::MatrixXd Q{Eigen::Matrix2d::Identity()};
  const Eigen::MatrixXd R{Eigen::Matrix2d::Identity()};
  const Eigen::MatrixXd N{Eigen::Matrix2d::Identity()};

  Eigen::MatrixXd X = frc::DARE(A, B, Q, R, N);
  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, N, X);
}
