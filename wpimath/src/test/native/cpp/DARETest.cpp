// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <stdexcept>

#include <Eigen/Core>
#include <gtest/gtest.h>

#include "DARETestUtil.h"
#include "frc/DARE.h"
#include "frc/EigenCore.h"
#include "frc/fmt/Eigen.h"

// 2x1
extern template Eigen::Matrix<double, 2, 2> frc::DARE<2, 1>(
    const Eigen::Matrix<double, 2, 2>& A, const Eigen::Matrix<double, 2, 1>& B,
    const Eigen::Matrix<double, 2, 2>& Q, const Eigen::Matrix<double, 1, 1>& R);
extern template Eigen::Matrix<double, 2, 2> frc::DARE<2, 1>(
    const Eigen::Matrix<double, 2, 2>& A, const Eigen::Matrix<double, 2, 1>& B,
    const Eigen::Matrix<double, 2, 2>& Q, const Eigen::Matrix<double, 1, 1>& R,
    const Eigen::Matrix<double, 2, 1>& N);

// 4x1
extern template Eigen::Matrix<double, 4, 4> frc::DARE<4, 1>(
    const Eigen::Matrix<double, 4, 4>& A, const Eigen::Matrix<double, 4, 1>& B,
    const Eigen::Matrix<double, 4, 4>& Q, const Eigen::Matrix<double, 1, 1>& R);
extern template Eigen::Matrix<double, 4, 4> frc::DARE<4, 1>(
    const Eigen::Matrix<double, 4, 4>& A, const Eigen::Matrix<double, 4, 1>& B,
    const Eigen::Matrix<double, 4, 4>& Q, const Eigen::Matrix<double, 1, 1>& R,
    const Eigen::Matrix<double, 4, 1>& N);

// 2x2
extern template Eigen::Matrix<double, 2, 2> frc::DARE<2, 2>(
    const Eigen::Matrix<double, 2, 2>& A, const Eigen::Matrix<double, 2, 2>& B,
    const Eigen::Matrix<double, 2, 2>& Q, const Eigen::Matrix<double, 2, 2>& R);
extern template Eigen::Matrix<double, 2, 2> frc::DARE<2, 2>(
    const Eigen::Matrix<double, 2, 2>& A, const Eigen::Matrix<double, 2, 2>& B,
    const Eigen::Matrix<double, 2, 2>& Q, const Eigen::Matrix<double, 2, 2>& R,
    const Eigen::Matrix<double, 2, 2>& N);

// 2x3
extern template Eigen::Matrix<double, 2, 2> frc::DARE<2, 3>(
    const Eigen::Matrix<double, 2, 2>& A, const Eigen::Matrix<double, 2, 3>& B,
    const Eigen::Matrix<double, 2, 2>& Q, const Eigen::Matrix<double, 3, 3>& R);
extern template Eigen::Matrix<double, 2, 2> frc::DARE<2, 3>(
    const Eigen::Matrix<double, 2, 2>& A, const Eigen::Matrix<double, 2, 3>& B,
    const Eigen::Matrix<double, 2, 2>& Q, const Eigen::Matrix<double, 3, 3>& R,
    const Eigen::Matrix<double, 2, 3>& N);

TEST(DARETest, NonInvertibleA_ABQR) {
  // Example 2 of "On the Numerical Solution of the Discrete-Time Algebraic
  // Riccati Equation"

  frc::Matrixd<4, 4> A{
      {0.5, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {0, 0, 0, 0}};
  frc::Matrixd<4, 1> B{{0}, {0}, {0}, {1}};
  frc::Matrixd<4, 4> Q{{1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
  frc::Matrixd<1, 1> R{0.25};

  frc::Matrixd<4, 4> X = frc::DARE<4, 1>(A, B, Q, R);
  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, X);
}

TEST(DARETest, NonInvertibleA_ABQRN) {
  // Example 2 of "On the Numerical Solution of the Discrete-Time Algebraic
  // Riccati Equation"

  frc::Matrixd<4, 4> A{
      {0.5, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {0, 0, 0, 0}};
  frc::Matrixd<4, 1> B{{0}, {0}, {0}, {1}};
  frc::Matrixd<4, 4> Q{{1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
  frc::Matrixd<1, 1> R{0.25};

  frc::Matrixd<4, 4> Aref{
      {0.25, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {0, 0, 0, 0}};
  Q = (A - Aref).transpose() * Q * (A - Aref);
  R = B.transpose() * Q * B + R;
  frc::Matrixd<4, 1> N = (A - Aref).transpose() * Q * B;

  frc::Matrixd<4, 4> X = frc::DARE<4, 1>(A, B, Q, R, N);
  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, N, X);
}

TEST(DARETest, InvertibleA_ABQR) {
  frc::Matrixd<2, 2> A{{1, 1}, {0, 1}};
  frc::Matrixd<2, 1> B{{0}, {1}};
  frc::Matrixd<2, 2> Q{{1, 0}, {0, 0}};
  frc::Matrixd<1, 1> R{{0.3}};

  frc::Matrixd<2, 2> X = frc::DARE<2, 1>(A, B, Q, R);
  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, X);
}

TEST(DARETest, InvertibleA_ABQRN) {
  frc::Matrixd<2, 2> A{{1, 1}, {0, 1}};
  frc::Matrixd<2, 1> B{{0}, {1}};
  frc::Matrixd<2, 2> Q{{1, 0}, {0, 0}};
  frc::Matrixd<1, 1> R{0.3};

  frc::Matrixd<2, 2> Aref{{0.5, 1}, {0, 1}};
  Q = (A - Aref).transpose() * Q * (A - Aref);
  R = B.transpose() * Q * B + R;
  frc::Matrixd<2, 1> N = (A - Aref).transpose() * Q * B;

  frc::Matrixd<2, 2> X = frc::DARE<2, 1>(A, B, Q, R, N);
  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, N, X);
}

TEST(DARETest, FirstGeneralizedEigenvalueOfSTIsStable_ABQR) {
  // The first generalized eigenvalue of (S, T) is stable

  frc::Matrixd<2, 2> A{{0, 1}, {0, 0}};
  frc::Matrixd<2, 1> B{{0}, {1}};
  frc::Matrixd<2, 2> Q{{1, 0}, {0, 1}};
  frc::Matrixd<1, 1> R{1};

  frc::Matrixd<2, 2> X = frc::DARE<2, 1>(A, B, Q, R);
  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, X);
}

TEST(DARETest, FirstGeneralizedEigenvalueOfSTIsStable_ABQRN) {
  // The first generalized eigenvalue of (S, T) is stable

  frc::Matrixd<2, 2> A{{0, 1}, {0, 0}};
  frc::Matrixd<2, 1> B{{0}, {1}};
  frc::Matrixd<2, 2> Q{{1, 0}, {0, 1}};
  frc::Matrixd<1, 1> R{1};

  frc::Matrixd<2, 2> Aref{{0, 0.5}, {0, 0}};
  Q = (A - Aref).transpose() * Q * (A - Aref);
  R = B.transpose() * Q * B + R;
  frc::Matrixd<2, 1> N = (A - Aref).transpose() * Q * B;

  frc::Matrixd<2, 2> X = frc::DARE<2, 1>(A, B, Q, R, N);
  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, N, X);
}

TEST(DARETest, IdentitySystem_ABQR) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};

  Eigen::Matrix2d X = frc::DARE<2, 2>(A, B, Q, R);
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

  Eigen::Matrix2d X = frc::DARE<2, 2>(A, B, Q, R, N);
  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, N, X);
}

TEST(DARETest, MoreInputsThanStates_ABQR) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const frc::Matrixd<2, 3> B{{1.0, 0.0, 0.0}, {0.0, 0.5, 0.3}};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix3d R{Eigen::Matrix3d::Identity()};

  Eigen::Matrix2d X = frc::DARE<2, 3>(A, B, Q, R);
  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, X);
}

TEST(DARETest, MoreInputsThanStates_ABQRN) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const frc::Matrixd<2, 3> B{{1.0, 0.0, 0.0}, {0.0, 0.5, 0.3}};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix3d R{Eigen::Matrix3d::Identity()};
  const frc::Matrixd<2, 3> N{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}};

  Eigen::Matrix2d X = frc::DARE<2, 3>(A, B, Q, R, N);
  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, N, X);
}

TEST(DARETest, QNotSymmetricPositiveSemidefinite_ABQR) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{-Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};

  EXPECT_THROW((frc::DARE<2, 2>(A, B, Q, R)), std::invalid_argument);
}

TEST(DARETest, QNotSymmetricPositiveSemidefinite_ABQRN) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d N{2.0 * Eigen::Matrix2d::Identity()};

  EXPECT_THROW((frc::DARE<2, 2>(A, B, Q, R, N)), std::invalid_argument);
}

TEST(DARETest, RNotSymmetricPositiveDefinite_ABQR) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};

  const Eigen::Matrix2d R1{Eigen::Matrix2d::Zero()};
  EXPECT_THROW((frc::DARE<2, 2>(A, B, Q, R1)), std::invalid_argument);

  const Eigen::Matrix2d R2{-Eigen::Matrix2d::Identity()};
  EXPECT_THROW((frc::DARE<2, 2>(A, B, Q, R2)), std::invalid_argument);
}

TEST(DARETest, RNotSymmetricPositiveDefinite_ABQRN) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d N{Eigen::Matrix2d::Identity()};

  const Eigen::Matrix2d R1{Eigen::Matrix2d::Zero()};
  EXPECT_THROW((frc::DARE<2, 2>(A, B, Q, R1, N)), std::invalid_argument);

  const Eigen::Matrix2d R2{-Eigen::Matrix2d::Identity()};
  EXPECT_THROW((frc::DARE<2, 2>(A, B, Q, R2, N)), std::invalid_argument);
}

TEST(DARETest, ABNotStabilizable_ABQR) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Zero()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};

  EXPECT_THROW((frc::DARE<2, 2>(A, B, Q, R)), std::invalid_argument);
}

TEST(DARETest, ABNotStabilizable_ABQRN) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Zero()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d N{Eigen::Matrix2d::Identity()};

  EXPECT_THROW((frc::DARE<2, 2>(A, B, Q, R, N)), std::invalid_argument);
}

TEST(DARETest, ACNotDetectable_ABQR) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Zero()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};

  EXPECT_THROW((frc::DARE<2, 2>(A, B, Q, R)), std::invalid_argument);
}

TEST(DARETest, ACNotDetectable_ABQRN) {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Zero()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d N{Eigen::Matrix2d::Zero()};

  EXPECT_THROW((frc::DARE<2, 2>(A, B, Q, R, N)), std::invalid_argument);
}

TEST(DARETest, QDecomposition) {
  // Ensures the decomposition of Q into CᵀC is correct

  const Eigen::Matrix2d A{{1.0, 0.0}, {0.0, 0.0}};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};

  // (A, C₁) should be detectable pair
  const Eigen::Matrix2d C_1{{0.0, 0.0}, {1.0, 0.0}};
  const Eigen::Matrix2d Q_1 = C_1.transpose() * C_1;
  EXPECT_NO_THROW((frc::DARE<2, 2>(A, B, Q_1, R)));

  // (A, C₂) shouldn't be detectable pair
  const Eigen::Matrix2d C_2 = C_1.transpose();
  const Eigen::Matrix2d Q_2 = C_2.transpose() * C_2;
  EXPECT_THROW((frc::DARE<2, 2>(A, B, Q_2, R)), std::invalid_argument);
}
