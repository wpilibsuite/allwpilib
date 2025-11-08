// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <stdexcept>

#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <gtest/gtest.h>

#include "wpi/math/fmt/Eigen.hpp"
#include "wpi/math/linalg/DARE.hpp"
#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/util/expected"
#include "wpi/util/print.hpp"

// 2x1
extern template wpi::util::expected<Eigen::Matrix<double, 2, 2>,
                                    wpi::math::DAREError>
wpi::math::DARE<2, 1>(const Eigen::Matrix<double, 2, 2>& A,
                      const Eigen::Matrix<double, 2, 1>& B,
                      const Eigen::Matrix<double, 2, 2>& Q,
                      const Eigen::Matrix<double, 1, 1>& R,
                      bool checkPreconditions);
extern template wpi::util::expected<Eigen::Matrix<double, 2, 2>,
                                    wpi::math::DAREError>
wpi::math::DARE<2, 1>(const Eigen::Matrix<double, 2, 2>& A,
                      const Eigen::Matrix<double, 2, 1>& B,
                      const Eigen::Matrix<double, 2, 2>& Q,
                      const Eigen::Matrix<double, 1, 1>& R,
                      const Eigen::Matrix<double, 2, 1>& N,
                      bool checkPreconditions);

// 4x1
extern template wpi::util::expected<Eigen::Matrix<double, 4, 4>,
                                    wpi::math::DAREError>
wpi::math::DARE<4, 1>(const Eigen::Matrix<double, 4, 4>& A,
                      const Eigen::Matrix<double, 4, 1>& B,
                      const Eigen::Matrix<double, 4, 4>& Q,
                      const Eigen::Matrix<double, 1, 1>& R,
                      bool checkPreconditions);
extern template wpi::util::expected<Eigen::Matrix<double, 4, 4>,
                                    wpi::math::DAREError>
wpi::math::DARE<4, 1>(const Eigen::Matrix<double, 4, 4>& A,
                      const Eigen::Matrix<double, 4, 1>& B,
                      const Eigen::Matrix<double, 4, 4>& Q,
                      const Eigen::Matrix<double, 1, 1>& R,
                      const Eigen::Matrix<double, 4, 1>& N,
                      bool checkPreconditions);

// 2x2
extern template wpi::util::expected<Eigen::Matrix<double, 2, 2>,
                                    wpi::math::DAREError>
wpi::math::DARE<2, 2>(const Eigen::Matrix<double, 2, 2>& A,
                      const Eigen::Matrix<double, 2, 2>& B,
                      const Eigen::Matrix<double, 2, 2>& Q,
                      const Eigen::Matrix<double, 2, 2>& R,
                      bool checkPreconditions);
extern template wpi::util::expected<Eigen::Matrix<double, 2, 2>,
                                    wpi::math::DAREError>
wpi::math::DARE<2, 2>(const Eigen::Matrix<double, 2, 2>& A,
                      const Eigen::Matrix<double, 2, 2>& B,
                      const Eigen::Matrix<double, 2, 2>& Q,
                      const Eigen::Matrix<double, 2, 2>& R,
                      const Eigen::Matrix<double, 2, 2>& N,
                      bool checkPreconditions);

// 2x3
extern template wpi::util::expected<Eigen::Matrix<double, 2, 2>,
                                    wpi::math::DAREError>
wpi::math::DARE<2, 3>(const Eigen::Matrix<double, 2, 2>& A,
                      const Eigen::Matrix<double, 2, 3>& B,
                      const Eigen::Matrix<double, 2, 2>& Q,
                      const Eigen::Matrix<double, 3, 3>& R,
                      bool checkPreconditions);
extern template wpi::util::expected<Eigen::Matrix<double, 2, 2>,
                                    wpi::math::DAREError>
wpi::math::DARE<2, 3>(const Eigen::Matrix<double, 2, 2>& A,
                      const Eigen::Matrix<double, 2, 3>& B,
                      const Eigen::Matrix<double, 2, 2>& Q,
                      const Eigen::Matrix<double, 3, 3>& R,
                      const Eigen::Matrix<double, 2, 3>& N,
                      bool checkPreconditions);

void ExpectMatrixEqual(const Eigen::MatrixXd& lhs, const Eigen::MatrixXd& rhs,
                       double tolerance) {
  for (int row = 0; row < lhs.rows(); ++row) {
    for (int col = 0; col < lhs.cols(); ++col) {
      EXPECT_NEAR(lhs(row, col), rhs(row, col), tolerance)
          << fmt::format("row = {}, col = {}", row, col);
    }
  }

  if (::testing::Test::HasFailure()) {
    wpi::util::print("lhs =\n{}\n", lhs);
    wpi::util::print("rhs =\n{}\n", rhs);
    wpi::util::print("delta =\n{}\n", Eigen::MatrixXd{lhs - rhs});
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

  wpi::math::Matrixd<4, 4> A{
      {0.5, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {0, 0, 0, 0}};
  wpi::math::Matrixd<4, 1> B{{0}, {0}, {0}, {1}};
  wpi::math::Matrixd<4, 4> Q{
      {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
  wpi::math::Matrixd<1, 1> R{0.25};

  auto ret = wpi::math::DARE<4, 1>(A, B, Q, R);
  EXPECT_TRUE(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, X);
}

TEST(DARETest, NonInvertibleA_ABQRN) {
  // Example 2 of "On the Numerical Solution of the Discrete-Time Algebraic
  // Riccati Equation"

  wpi::math::Matrixd<4, 4> A{
      {0.5, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {0, 0, 0, 0}};
  wpi::math::Matrixd<4, 1> B{{0}, {0}, {0}, {1}};
  wpi::math::Matrixd<4, 4> Q{
      {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
  wpi::math::Matrixd<1, 1> R{0.25};

  wpi::math::Matrixd<4, 4> Aref{
      {0.25, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {0, 0, 0, 0}};
  Q = (A - Aref).transpose() * Q * (A - Aref);
  R = B.transpose() * Q * B + R;
  wpi::math::Matrixd<4, 1> N = (A - Aref).transpose() * Q * B;

  auto ret = wpi::math::DARE<4, 1>(A, B, Q, R, N);
  EXPECT_TRUE(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, N, X);
}

TEST(DARETest, InvertibleA_ABQR) {
  wpi::math::Matrixd<2, 2> A{{1, 1}, {0, 1}};
  wpi::math::Matrixd<2, 1> B{{0}, {1}};
  wpi::math::Matrixd<2, 2> Q{{1, 0}, {0, 0}};
  wpi::math::Matrixd<1, 1> R{{0.3}};

  auto ret = wpi::math::DARE<2, 1>(A, B, Q, R);
  EXPECT_TRUE(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, X);
}

TEST(DARETest, InvertibleA_ABQRN) {
  wpi::math::Matrixd<2, 2> A{{1, 1}, {0, 1}};
  wpi::math::Matrixd<2, 1> B{{0}, {1}};
  wpi::math::Matrixd<2, 2> Q{{1, 0}, {0, 0}};
  wpi::math::Matrixd<1, 1> R{0.3};

  wpi::math::Matrixd<2, 2> Aref{{0.5, 1}, {0, 1}};
  Q = (A - Aref).transpose() * Q * (A - Aref);
  R = B.transpose() * Q * B + R;
  wpi::math::Matrixd<2, 1> N = (A - Aref).transpose() * Q * B;

  auto ret = wpi::math::DARE<2, 1>(A, B, Q, R, N);
  EXPECT_TRUE(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, N, X);
}

TEST(DARETest, FirstGeneralizedEigenvalueOfSTIsStable_ABQR) {
  // The first generalized eigenvalue of (S, T) is stable

  wpi::math::Matrixd<2, 2> A{{0, 1}, {0, 0}};
  wpi::math::Matrixd<2, 1> B{{0}, {1}};
  wpi::math::Matrixd<2, 2> Q{{1, 0}, {0, 1}};
  wpi::math::Matrixd<1, 1> R{1};

  auto ret = wpi::math::DARE<2, 1>(A, B, Q, R);
  EXPECT_TRUE(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, X);
}

TEST(DARETest, FirstGeneralizedEigenvalueOfSTIsStable_ABQRN) {
  // The first generalized eigenvalue of (S, T) is stable

  wpi::math::Matrixd<2, 2> A{{0, 1}, {0, 0}};
  wpi::math::Matrixd<2, 1> B{{0}, {1}};
  wpi::math::Matrixd<2, 2> Q{{1, 0}, {0, 1}};
  wpi::math::Matrixd<1, 1> R{1};

  wpi::math::Matrixd<2, 2> Aref{{0, 0.5}, {0, 0}};
  Q = (A - Aref).transpose() * Q * (A - Aref);
  R = B.transpose() * Q * B + R;
  wpi::math::Matrixd<2, 1> N = (A - Aref).transpose() * Q * B;

  auto ret = wpi::math::DARE<2, 1>(A, B, Q, R, N);
  EXPECT_TRUE(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, N, X);
}

TEST(DARETest, IdentitySystem_ABQR) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R);
  EXPECT_TRUE(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, X);
}

TEST(DARETest, IdentitySystem_ABQRN) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d N{Eigen::Matrix2d::Identity()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R, N);
  EXPECT_TRUE(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, N, X);
}

TEST(DARETest, MoreInputsThanStates_ABQR) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const wpi::math::Matrixd<2, 3> B{{1.0, 0.0, 0.0}, {0.0, 0.5, 0.3}};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix3d R{Eigen::Matrix3d::Identity()};

  auto ret = wpi::math::DARE<2, 3>(A, B, Q, R);
  EXPECT_TRUE(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, X);
}

TEST(DARETest, MoreInputsThanStates_ABQRN) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const wpi::math::Matrixd<2, 3> B{{1.0, 0.0, 0.0}, {0.0, 0.5, 0.3}};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix3d R{Eigen::Matrix3d::Identity()};
  const wpi::math::Matrixd<2, 3> N{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}};

  auto ret = wpi::math::DARE<2, 3>(A, B, Q, R, N);
  EXPECT_TRUE(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, N, X);
}

TEST(DARETest, QNotSymmetric_ABQR) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{{1.0, 1.0}, {0.0, 1.0}};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R);
  EXPECT_FALSE(ret);
  EXPECT_EQ(ret.error(), wpi::math::DAREError::QNotSymmetric);
}

TEST(DARETest, QNotPositiveSemidefinite_ABQR) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{-Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R);
  EXPECT_FALSE(ret);
  EXPECT_EQ(ret.error(), wpi::math::DAREError::QNotPositiveSemidefinite);
}

TEST(DARETest, QNotSymmetric_ABQRN) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{{1.0, 1.0}, {0.0, 1.0}};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d N{2.0 * Eigen::Matrix2d::Identity()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R, N);
  EXPECT_FALSE(ret);
  EXPECT_EQ(ret.error(), wpi::math::DAREError::QNotSymmetric);
}

TEST(DARETest, QNotPositiveSemidefinite_ABQRN) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d N{2.0 * Eigen::Matrix2d::Identity()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R, N);
  EXPECT_FALSE(ret);
  EXPECT_EQ(ret.error(), wpi::math::DAREError::QNotPositiveSemidefinite);
}

TEST(DARETest, RNotSymmetric_ABQR) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{{1.0, 1.0}, {0.0, 1.0}};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R);
  EXPECT_FALSE(ret);
  EXPECT_EQ(ret.error(), wpi::math::DAREError::RNotSymmetric);
}

TEST(DARETest, RNotPositiveDefinite_ABQR) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};

  const Eigen::Matrix2d R1{Eigen::Matrix2d::Zero()};
  auto ret1 = wpi::math::DARE<2, 2>(A, B, Q, R1);
  EXPECT_FALSE(ret1);
  EXPECT_EQ(ret1.error(), wpi::math::DAREError::RNotPositiveDefinite);

  const Eigen::Matrix2d R2{-Eigen::Matrix2d::Identity()};
  auto ret2 = wpi::math::DARE<2, 2>(A, B, Q, R2);
  EXPECT_FALSE(ret2);
  EXPECT_EQ(ret2.error(), wpi::math::DAREError::RNotPositiveDefinite);
}

TEST(DARETest, RNotSymmetric_ABQRN) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d N{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{{1.0, 1.0}, {0.0, 1.0}};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R, N);
  EXPECT_FALSE(ret);
  EXPECT_EQ(ret.error(), wpi::math::DAREError::RNotSymmetric);
}

TEST(DARETest, RNotPositiveDefinite_ABQRN) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d N{Eigen::Matrix2d::Identity()};

  const Eigen::Matrix2d R1{Eigen::Matrix2d::Zero()};
  auto ret1 = wpi::math::DARE<2, 2>(A, B, Q, R1, N);
  EXPECT_FALSE(ret1);
  EXPECT_EQ(ret1.error(), wpi::math::DAREError::RNotPositiveDefinite);

  const Eigen::Matrix2d R2{-Eigen::Matrix2d::Identity()};
  auto ret2 = wpi::math::DARE<2, 2>(A, B, Q, R2, N);
  EXPECT_FALSE(ret2);
  EXPECT_EQ(ret2.error(), wpi::math::DAREError::RNotPositiveDefinite);
}

TEST(DARETest, ABNotStabilizable_ABQR) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Zero()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R);
  EXPECT_FALSE(ret);
  EXPECT_EQ(ret.error(), wpi::math::DAREError::ABNotStabilizable);
}

TEST(DARETest, ABNotStabilizable_ABQRN) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Zero()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d N{Eigen::Matrix2d::Identity()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R, N);
  EXPECT_FALSE(ret);
  EXPECT_EQ(ret.error(), wpi::math::DAREError::ABNotStabilizable);
}

TEST(DARETest, ACNotDetectable_ABQR) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Zero()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R);
  EXPECT_FALSE(ret);
  EXPECT_EQ(ret.error(), wpi::math::DAREError::ACNotDetectable);
}

TEST(DARETest, ACNotDetectable_ABQRN) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Zero()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d N{Eigen::Matrix2d::Zero()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R, N);
  EXPECT_FALSE(ret);
  EXPECT_EQ(ret.error(), wpi::math::DAREError::ACNotDetectable);
}

TEST(DARETest, QDecomposition) {
  // Ensures the decomposition of Q into CᵀC is correct

  const Eigen::Matrix2d A{{1.0, 0.0}, {0.0, 0.0}};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};

  // (A, C₁) should be detectable pair
  const Eigen::Matrix2d C_1{{0.0, 0.0}, {1.0, 0.0}};
  const Eigen::Matrix2d Q_1 = C_1.transpose() * C_1;
  auto ret1 = wpi::math::DARE<2, 2>(A, B, Q_1, R);
  EXPECT_TRUE(ret1);

  // (A, C₂) shouldn't be detectable pair
  const Eigen::Matrix2d C_2 = C_1.transpose();
  const Eigen::Matrix2d Q_2 = C_2.transpose() * C_2;
  auto ret2 = wpi::math::DARE<2, 2>(A, B, Q_2, R);
  EXPECT_FALSE(ret2);
}
