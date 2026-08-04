// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/linalg/DARE.hpp"

#include <algorithm>
#include <cmath>
#include <expected>
#include <format>

#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/math/fmt/Eigen.hpp"
#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/util/print.hpp"

// 2x1
extern template std::expected<Eigen::Matrix<double, 2, 2>, wpi::math::DAREError>
wpi::math::DARE<2, 1>(const Eigen::Matrix<double, 2, 2>& A,
                      const Eigen::Matrix<double, 2, 1>& B,
                      const Eigen::Matrix<double, 2, 2>& Q,
                      const Eigen::Matrix<double, 1, 1>& R,
                      bool checkPreconditions);
extern template std::expected<Eigen::Matrix<double, 2, 2>, wpi::math::DAREError>
wpi::math::DARE<2, 1>(const Eigen::Matrix<double, 2, 2>& A,
                      const Eigen::Matrix<double, 2, 1>& B,
                      const Eigen::Matrix<double, 2, 2>& Q,
                      const Eigen::Matrix<double, 1, 1>& R,
                      const Eigen::Matrix<double, 2, 1>& N,
                      bool checkPreconditions);

// 4x1
extern template std::expected<Eigen::Matrix<double, 4, 4>, wpi::math::DAREError>
wpi::math::DARE<4, 1>(const Eigen::Matrix<double, 4, 4>& A,
                      const Eigen::Matrix<double, 4, 1>& B,
                      const Eigen::Matrix<double, 4, 4>& Q,
                      const Eigen::Matrix<double, 1, 1>& R,
                      bool checkPreconditions);
extern template std::expected<Eigen::Matrix<double, 4, 4>, wpi::math::DAREError>
wpi::math::DARE<4, 1>(const Eigen::Matrix<double, 4, 4>& A,
                      const Eigen::Matrix<double, 4, 1>& B,
                      const Eigen::Matrix<double, 4, 4>& Q,
                      const Eigen::Matrix<double, 1, 1>& R,
                      const Eigen::Matrix<double, 4, 1>& N,
                      bool checkPreconditions);

// 2x2
extern template std::expected<Eigen::Matrix<double, 2, 2>, wpi::math::DAREError>
wpi::math::DARE<2, 2>(const Eigen::Matrix<double, 2, 2>& A,
                      const Eigen::Matrix<double, 2, 2>& B,
                      const Eigen::Matrix<double, 2, 2>& Q,
                      const Eigen::Matrix<double, 2, 2>& R,
                      bool checkPreconditions);
extern template std::expected<Eigen::Matrix<double, 2, 2>, wpi::math::DAREError>
wpi::math::DARE<2, 2>(const Eigen::Matrix<double, 2, 2>& A,
                      const Eigen::Matrix<double, 2, 2>& B,
                      const Eigen::Matrix<double, 2, 2>& Q,
                      const Eigen::Matrix<double, 2, 2>& R,
                      const Eigen::Matrix<double, 2, 2>& N,
                      bool checkPreconditions);

// 2x3
extern template std::expected<Eigen::Matrix<double, 2, 2>, wpi::math::DAREError>
wpi::math::DARE<2, 3>(const Eigen::Matrix<double, 2, 2>& A,
                      const Eigen::Matrix<double, 2, 3>& B,
                      const Eigen::Matrix<double, 2, 2>& Q,
                      const Eigen::Matrix<double, 3, 3>& R,
                      bool checkPreconditions);
extern template std::expected<Eigen::Matrix<double, 2, 2>, wpi::math::DAREError>
wpi::math::DARE<2, 3>(const Eigen::Matrix<double, 2, 2>& A,
                      const Eigen::Matrix<double, 2, 3>& B,
                      const Eigen::Matrix<double, 2, 2>& Q,
                      const Eigen::Matrix<double, 3, 3>& R,
                      const Eigen::Matrix<double, 2, 3>& N,
                      bool checkPreconditions);

constexpr double kDARENormalizedResidualTolerance = 1e-12;
constexpr double kQuadcopterDt = 1e-4;
constexpr int kQuadcopterScaleExponent = 44;

void ExpectMatrixEqual(const Eigen::MatrixXd& lhs, const Eigen::MatrixXd& rhs,
                       double tolerance) {
  for (int row = 0; row < lhs.rows(); ++row) {
    for (int col = 0; col < lhs.cols(); ++col) {
      UNSCOPED_INFO("row = " << row << ", col = " << col << "\nlhs:\n"
                             << lhs << "\nrhs:\n"
                             << rhs);
      CHECK_NEAR(lhs(row, col), rhs(row, col), tolerance);
    }
  }
}

void ExpectPositiveSemidefinite(const Eigen::MatrixXd& X) {
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigX{X};
  CHECK(eigX.info() == Eigen::Success);

  for (int i = 0; i < X.rows(); ++i) {
    CHECK(eigX.eigenvalues()[i] >= 0.0);
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

double DARENormalizedResidualFromTerms(
    const Eigen::Ref<const Eigen::MatrixXd>& A,
    const Eigen::Ref<const Eigen::MatrixXd>& Q,
    const Eigen::Ref<const Eigen::MatrixXd>& X,
    const Eigen::Ref<const Eigen::MatrixXd>& inputCost,
    const Eigen::Ref<const Eigen::MatrixXd>& leftTerm,
    const Eigen::Ref<const Eigen::MatrixXd>& rightTerm) {
  Eigen::MatrixXd stateTerm = A.transpose() * X * A;
  Eigen::MatrixXd feedbackTerm = leftTerm * inputCost.llt().solve(rightTerm);
  Eigen::MatrixXd residual = stateTerm - X - feedbackTerm + Q;
  double normalizer = std::max(
      {1.0, stateTerm.norm(), X.norm(), feedbackTerm.norm(), Q.norm()});

  return residual.norm() / normalizer;
}

double DARENormalizedResidual(const Eigen::Ref<const Eigen::MatrixXd>& A,
                              const Eigen::Ref<const Eigen::MatrixXd>& B,
                              const Eigen::Ref<const Eigen::MatrixXd>& Q,
                              const Eigen::Ref<const Eigen::MatrixXd>& R,
                              const Eigen::Ref<const Eigen::MatrixXd>& X) {
  Eigen::MatrixXd inputCost = B.transpose() * X * B + R;
  Eigen::MatrixXd stateInput = A.transpose() * X * B;
  Eigen::MatrixXd rightTerm = stateInput.transpose();

  return DARENormalizedResidualFromTerms(A, Q, X, inputCost, stateInput,
                                         rightTerm);
}

double DARENormalizedResidual(const Eigen::Ref<const Eigen::MatrixXd>& A,
                              const Eigen::Ref<const Eigen::MatrixXd>& B,
                              const Eigen::Ref<const Eigen::MatrixXd>& Q,
                              const Eigen::Ref<const Eigen::MatrixXd>& R,
                              const Eigen::Ref<const Eigen::MatrixXd>& N,
                              const Eigen::Ref<const Eigen::MatrixXd>& X) {
  Eigen::MatrixXd inputCost = B.transpose() * X * B + R;
  Eigen::MatrixXd leftTerm = A.transpose() * X * B + N;
  Eigen::MatrixXd rightTerm = B.transpose() * X * A + N.transpose();

  return DARENormalizedResidualFromTerms(A, Q, X, inputCost, leftTerm,
                                         rightTerm);
}

wpi::math::Matrixd<10, 10> MakeQuadcopterA() {
  wpi::math::Matrixd<10, 10> A = wpi::math::Matrixd<10, 10>::Zero();
  A(0, 0) = 0.9999;
  A(0, 3) = 1e-8;

  double stable = std::exp(-kQuadcopterDt);
  double unstable = std::exp(6.0 * kQuadcopterDt);
  A(3, 3) = stable;
  A(4, 4) = stable;
  A(5, 5) = stable;
  A(6, 6) = unstable;
  A(7, 7) = unstable;
  A(8, 8) = unstable;
  A(9, 9) = unstable;

  return A;
}

wpi::math::Matrixd<10, 4> MakeQuadcopterB() {
  wpi::math::Matrixd<10, 4> B = wpi::math::Matrixd<10, 4>::Zero();
  double stableInput = 1.0 - std::exp(-kQuadcopterDt);
  double unstableInput = (std::exp(6.0 * kQuadcopterDt) - 1.0) / 6.0;
  B.row(3).setConstant(stableInput);
  B(6, 0) = unstableInput;
  B(7, 1) = unstableInput;
  B(8, 2) = unstableInput;
  B(9, 3) = unstableInput;

  return B;
}

wpi::math::Matrixd<10, 10> MakeStateTransform() {
  wpi::math::Matrixd<10, 10> C = wpi::math::Matrixd<10, 10>::Identity();
  C(0, 0) = std::ldexp(1.0, kQuadcopterScaleExponent);
  return C;
}

struct QuadcopterDAREProblem {
  wpi::math::Matrixd<10, 10> A;
  wpi::math::Matrixd<10, 4> B;
  wpi::math::Matrixd<10, 10> Q;
  wpi::math::Matrixd<4, 4> R;
};

QuadcopterDAREProblem MakeCoordinateTransformedQuadcopterDAREProblem() {
  auto A = MakeQuadcopterA();
  auto B = MakeQuadcopterB();
  wpi::math::Matrixd<10, 10> Q = wpi::math::Matrixd<10, 10>::Identity();
  wpi::math::Matrixd<4, 4> R = wpi::math::Matrixd<4, 4>::Identity();

  auto C = MakeStateTransform();
  auto CInv = C.inverse();

  // Q = I keeps the deliberately unstable modes detectable. The 2^44 state
  // transform recreates the badly scaled repro using exactly representable
  // powers of two.
  return {CInv * A * C, CInv * B, C.transpose() * Q * C, R};
}

wpi::math::Matrixd<10, 4> MakeQuadcopterN() {
  wpi::math::Matrixd<10, 4> N = wpi::math::Matrixd<10, 4>::Zero();
  N(3, 0) = 0.125;
  N(6, 0) = -0.0625;
  N(7, 1) = 0.03125;
  N(8, 2) = -0.015625;
  N(9, 3) = 0.0078125;

  return N;
}

void ExpectDAREFixedPoint(const Eigen::Ref<const Eigen::MatrixXd>& A,
                          const Eigen::Ref<const Eigen::MatrixXd>& B,
                          const Eigen::Ref<const Eigen::MatrixXd>& Q,
                          const Eigen::Ref<const Eigen::MatrixXd>& R,
                          const Eigen::Ref<const Eigen::MatrixXd>& X) {
  REQUIRE(X.allFinite());

  double residual = DARENormalizedResidual(A, B, Q, R, X);
  UNSCOPED_INFO("normalized residual = " << residual);
  CHECK(residual < kDARENormalizedResidualTolerance);
}

void ExpectDAREFixedPoint(const Eigen::Ref<const Eigen::MatrixXd>& A,
                          const Eigen::Ref<const Eigen::MatrixXd>& B,
                          const Eigen::Ref<const Eigen::MatrixXd>& Q,
                          const Eigen::Ref<const Eigen::MatrixXd>& R,
                          const Eigen::Ref<const Eigen::MatrixXd>& N,
                          const Eigen::Ref<const Eigen::MatrixXd>& X) {
  REQUIRE(X.allFinite());

  double residual = DARENormalizedResidual(A, B, Q, R, N, X);
  UNSCOPED_INFO("normalized residual = " << residual);
  CHECK(residual < kDARENormalizedResidualTolerance);
}

template <int States, int Inputs>
void ExpectDAREFixedPoints(const Eigen::Matrix<double, States, States>& A,
                           const Eigen::Matrix<double, States, Inputs>& B,
                           const Eigen::Matrix<double, States, States>& Q,
                           const Eigen::Matrix<double, Inputs, Inputs>& R) {
  auto checkedRet = wpi::math::DARE<States, Inputs>(A, B, Q, R);
  REQUIRE(checkedRet);
  ExpectDAREFixedPoint(A, B, Q, R, checkedRet.value());

  auto noPrecondRet = wpi::math::DARE<States, Inputs>(A, B, Q, R, false);
  REQUIRE(noPrecondRet);
  ExpectDAREFixedPoint(A, B, Q, R, noPrecondRet.value());
}

template <int States, int Inputs>
void ExpectDAREFixedPoints(const Eigen::Matrix<double, States, States>& A,
                           const Eigen::Matrix<double, States, Inputs>& B,
                           const Eigen::Matrix<double, States, States>& Q,
                           const Eigen::Matrix<double, Inputs, Inputs>& R,
                           const Eigen::Matrix<double, States, Inputs>& N) {
  auto checkedRet = wpi::math::DARE<States, Inputs>(A, B, Q, R, N);
  REQUIRE(checkedRet);
  ExpectDAREFixedPoint(A, B, Q, R, N, checkedRet.value());

  auto noPrecondRet = wpi::math::DARE<States, Inputs>(A, B, Q, R, N, false);
  REQUIRE(noPrecondRet);
  ExpectDAREFixedPoint(A, B, Q, R, N, noPrecondRet.value());
}

TEST_CASE("DARETest NonInvertibleA ABQR", "[wpimath]") {
  // Example 2 of "On the Numerical Solution of the Discrete-Time Algebraic
  // Riccati Equation"

  wpi::math::Matrixd<4, 4> A{
      {0.5, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {0, 0, 0, 0}};
  wpi::math::Matrixd<4, 1> B{{0}, {0}, {0}, {1}};
  wpi::math::Matrixd<4, 4> Q{
      {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
  wpi::math::Matrixd<1, 1> R{0.25};

  auto ret = wpi::math::DARE<4, 1>(A, B, Q, R);
  CHECK(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, X);
}

TEST_CASE("DARETest NonInvertibleA ABQRN", "[wpimath]") {
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
  CHECK(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, N, X);
}

TEST_CASE("DARETest InvertibleA ABQR", "[wpimath]") {
  wpi::math::Matrixd<2, 2> A{{1, 1}, {0, 1}};
  wpi::math::Matrixd<2, 1> B{{0}, {1}};
  wpi::math::Matrixd<2, 2> Q{{1, 0}, {0, 0}};
  wpi::math::Matrixd<1, 1> R{{0.3}};

  auto ret = wpi::math::DARE<2, 1>(A, B, Q, R);
  CHECK(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, X);
}

TEST_CASE("DARETest InvertibleA ABQRN", "[wpimath]") {
  wpi::math::Matrixd<2, 2> A{{1, 1}, {0, 1}};
  wpi::math::Matrixd<2, 1> B{{0}, {1}};
  wpi::math::Matrixd<2, 2> Q{{1, 0}, {0, 0}};
  wpi::math::Matrixd<1, 1> R{0.3};

  wpi::math::Matrixd<2, 2> Aref{{0.5, 1}, {0, 1}};
  Q = (A - Aref).transpose() * Q * (A - Aref);
  R = B.transpose() * Q * B + R;
  wpi::math::Matrixd<2, 1> N = (A - Aref).transpose() * Q * B;

  auto ret = wpi::math::DARE<2, 1>(A, B, Q, R, N);
  CHECK(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, N, X);
}

TEST_CASE("DARETest FirstGeneralizedEigenvalueOfSTIsStable ABQR", "[wpimath]") {
  // The first generalized eigenvalue of (S, T) is stable

  wpi::math::Matrixd<2, 2> A{{0, 1}, {0, 0}};
  wpi::math::Matrixd<2, 1> B{{0}, {1}};
  wpi::math::Matrixd<2, 2> Q{{1, 0}, {0, 1}};
  wpi::math::Matrixd<1, 1> R{1};

  auto ret = wpi::math::DARE<2, 1>(A, B, Q, R);
  CHECK(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, X);
}

TEST_CASE("DARETest FirstGeneralizedEigenvalueOfSTIsStable ABQRN",
          "[wpimath]") {
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
  CHECK(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, N, X);
}

TEST_CASE("DARETest IdentitySystem ABQR", "[wpimath]") {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R);
  CHECK(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, X);
}

TEST_CASE("DARETest IdentitySystem ABQRN", "[wpimath]") {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d N{Eigen::Matrix2d::Identity()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R, N);
  CHECK(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, N, X);
}

TEST_CASE("DARETest MoreInputsThanStates ABQR", "[wpimath]") {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const wpi::math::Matrixd<2, 3> B{{1.0, 0.0, 0.0}, {0.0, 0.5, 0.3}};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix3d R{Eigen::Matrix3d::Identity()};

  auto ret = wpi::math::DARE<2, 3>(A, B, Q, R);
  CHECK(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, X);
}

TEST_CASE("DARETest MoreInputsThanStates ABQRN", "[wpimath]") {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const wpi::math::Matrixd<2, 3> B{{1.0, 0.0, 0.0}, {0.0, 0.5, 0.3}};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix3d R{Eigen::Matrix3d::Identity()};
  const wpi::math::Matrixd<2, 3> N{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}};

  auto ret = wpi::math::DARE<2, 3>(A, B, Q, R, N);
  CHECK(ret);
  auto X = ret.value();

  ExpectMatrixEqual(X, X.transpose(), 1e-10);
  ExpectPositiveSemidefinite(X);
  ExpectDARESolution(A, B, Q, R, N, X);
}

TEST_CASE("DARETest CoordinateTransformedQuadcopter ABQR", "[wpimath]") {
  auto problem = MakeCoordinateTransformedQuadcopterDAREProblem();

  ExpectDAREFixedPoints(problem.A, problem.B, problem.Q, problem.R);
}

TEST_CASE("DARETest CoordinateTransformedQuadcopter ABQRN", "[wpimath]") {
  auto problem = MakeCoordinateTransformedQuadcopterDAREProblem();
  auto N = MakeQuadcopterN();
  wpi::math::Matrixd<10, 10> AInput = problem.A + problem.B * N.transpose();
  wpi::math::Matrixd<10, 10> QInput = problem.Q + N * N.transpose();

  ExpectDAREFixedPoints(AInput, problem.B, QInput, problem.R, N);
}

TEST_CASE("DARETest QNotSymmetric ABQR", "[wpimath]") {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{{1.0, 1.0}, {0.0, 1.0}};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R);
  CHECK_FALSE(ret);
  CHECK(ret.error() == wpi::math::DAREError::QNotSymmetric);
}

TEST_CASE("DARETest QNotPositiveSemidefinite ABQR", "[wpimath]") {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{-Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R);
  CHECK_FALSE(ret);
  CHECK(ret.error() == wpi::math::DAREError::QNotPositiveSemidefinite);
}

TEST_CASE("DARETest QNotSymmetric ABQRN", "[wpimath]") {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{{1.0, 1.0}, {0.0, 1.0}};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d N{2.0 * Eigen::Matrix2d::Identity()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R, N);
  CHECK_FALSE(ret);
  CHECK(ret.error() == wpi::math::DAREError::QNotSymmetric);
}

TEST_CASE("DARETest QNotPositiveSemidefinite ABQRN", "[wpimath]") {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d N{2.0 * Eigen::Matrix2d::Identity()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R, N);
  CHECK_FALSE(ret);
  CHECK(ret.error() == wpi::math::DAREError::QNotPositiveSemidefinite);
}

TEST_CASE("DARETest RNotSymmetric ABQR", "[wpimath]") {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{{1.0, 1.0}, {0.0, 1.0}};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R);
  CHECK_FALSE(ret);
  CHECK(ret.error() == wpi::math::DAREError::RNotSymmetric);
}

TEST_CASE("DARETest RNotPositiveDefinite ABQR", "[wpimath]") {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};

  const Eigen::Matrix2d R1{Eigen::Matrix2d::Zero()};
  auto ret1 = wpi::math::DARE<2, 2>(A, B, Q, R1);
  CHECK_FALSE(ret1);
  CHECK(ret1.error() == wpi::math::DAREError::RNotPositiveDefinite);

  const Eigen::Matrix2d R2{-Eigen::Matrix2d::Identity()};
  auto ret2 = wpi::math::DARE<2, 2>(A, B, Q, R2);
  CHECK_FALSE(ret2);
  CHECK(ret2.error() == wpi::math::DAREError::RNotPositiveDefinite);
}

TEST_CASE("DARETest RNotSymmetric ABQRN", "[wpimath]") {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d N{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{{1.0, 1.0}, {0.0, 1.0}};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R, N);
  CHECK_FALSE(ret);
  CHECK(ret.error() == wpi::math::DAREError::RNotSymmetric);
}

TEST_CASE("DARETest RNotPositiveDefinite ABQRN", "[wpimath]") {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d N{Eigen::Matrix2d::Identity()};

  const Eigen::Matrix2d R1{Eigen::Matrix2d::Zero()};
  auto ret1 = wpi::math::DARE<2, 2>(A, B, Q, R1, N);
  CHECK_FALSE(ret1);
  CHECK(ret1.error() == wpi::math::DAREError::RNotPositiveDefinite);

  const Eigen::Matrix2d R2{-Eigen::Matrix2d::Identity()};
  auto ret2 = wpi::math::DARE<2, 2>(A, B, Q, R2, N);
  CHECK_FALSE(ret2);
  CHECK(ret2.error() == wpi::math::DAREError::RNotPositiveDefinite);
}

TEST_CASE("DARETest ABNotStabilizable ABQR", "[wpimath]") {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Zero()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R);
  CHECK_FALSE(ret);
  CHECK(ret.error() == wpi::math::DAREError::ABNotStabilizable);
}

TEST_CASE("DARETest ABNotStabilizable ABQRN", "[wpimath]") {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Zero()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d N{Eigen::Matrix2d::Identity()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R, N);
  CHECK_FALSE(ret);
  CHECK(ret.error() == wpi::math::DAREError::ABNotStabilizable);
}

TEST_CASE("DARETest ACNotDetectable ABQR", "[wpimath]") {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Zero()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R);
  CHECK_FALSE(ret);
  CHECK(ret.error() == wpi::math::DAREError::ACNotDetectable);
}

TEST_CASE("DARETest ACNotDetectable ABQRN", "[wpimath]") {
  const Eigen::Matrix2d A{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d Q{Eigen::Matrix2d::Zero()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d N{Eigen::Matrix2d::Zero()};

  auto ret = wpi::math::DARE<2, 2>(A, B, Q, R, N);
  CHECK_FALSE(ret);
  CHECK(ret.error() == wpi::math::DAREError::ACNotDetectable);
}

TEST_CASE("DARETest QDecomposition", "[wpimath]") {
  // Ensures the decomposition of Q into CᵀC is correct

  const Eigen::Matrix2d A{{1.0, 0.0}, {0.0, 0.0}};
  const Eigen::Matrix2d B{Eigen::Matrix2d::Identity()};
  const Eigen::Matrix2d R{Eigen::Matrix2d::Identity()};

  // (A, C₁) should be detectable pair
  const Eigen::Matrix2d C_1{{0.0, 0.0}, {1.0, 0.0}};
  const Eigen::Matrix2d Q_1 = C_1.transpose() * C_1;
  auto ret1 = wpi::math::DARE<2, 2>(A, B, Q_1, R);
  CHECK(ret1);

  // (A, C₂) shouldn't be detectable pair
  const Eigen::Matrix2d C_2 = C_1.transpose();
  const Eigen::Matrix2d Q_2 = C_2.transpose() * C_2;
  auto ret2 = wpi::math::DARE<2, 2>(A, B, Q_2, R);
  CHECK_FALSE(ret2);
}
