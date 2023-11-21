// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <Eigen/Cholesky>

#include "frc/StateSpaceUtil.h"
#include "frc/estimator/UnscentedKalmanFilter.h"
#include "frc/estimator/UnscentedTransform.h"
#include "frc/system/Discretization.h"
#include "frc/system/NumericalIntegration.h"
#include "frc/system/NumericalJacobian.h"

namespace frc {

template <int States, int Inputs, int Outputs>
UnscentedKalmanFilter<States, Inputs, Outputs>::UnscentedKalmanFilter(
    std::function<StateVector(const StateVector&, const InputVector&)> f,
    std::function<OutputVector(const StateVector&, const InputVector&)> h,
    const StateArray& stateStdDevs, const OutputArray& measurementStdDevs,
    units::second_t dt)
    : m_f(f), m_h(h) {
  m_contQ = MakeCovMatrix(stateStdDevs);
  m_contR = MakeCovMatrix(measurementStdDevs);
  m_meanFuncX = [](auto sigmas, auto Wm) -> StateVector { return sigmas * Wm; };
  m_meanFuncY = [](auto sigmas, auto Wc) -> OutputVector {
    return sigmas * Wc;
  };
  m_residualFuncX = [](auto a, auto b) -> StateVector { return a - b; };
  m_residualFuncY = [](auto a, auto b) -> OutputVector { return a - b; };
  m_addFuncX = [](auto a, auto b) -> StateVector { return a + b; };
  m_dt = dt;

  Reset();
}

template <int States, int Inputs, int Outputs>
UnscentedKalmanFilter<States, Inputs, Outputs>::UnscentedKalmanFilter(
    std::function<StateVector(const StateVector&, const InputVector&)> f,
    std::function<OutputVector(const StateVector&, const InputVector&)> h,
    const StateArray& stateStdDevs, const OutputArray& measurementStdDevs,
    std::function<StateVector(const Matrixd<States, 2 * States + 1>&,
                              const Vectord<2 * States + 1>&)>
        meanFuncX,
    std::function<OutputVector(const Matrixd<Outputs, 2 * States + 1>&,
                               const Vectord<2 * States + 1>&)>
        meanFuncY,
    std::function<StateVector(const StateVector&, const StateVector&)>
        residualFuncX,
    std::function<OutputVector(const OutputVector&, const OutputVector&)>
        residualFuncY,
    std::function<StateVector(const StateVector&, const StateVector&)> addFuncX,
    units::second_t dt)
    : m_f(f),
      m_h(h),
      m_meanFuncX(meanFuncX),
      m_meanFuncY(meanFuncY),
      m_residualFuncX(residualFuncX),
      m_residualFuncY(residualFuncY),
      m_addFuncX(addFuncX) {
  m_contQ = MakeCovMatrix(stateStdDevs);
  m_contR = MakeCovMatrix(measurementStdDevs);
  m_dt = dt;

  Reset();
}

template <int States, int Inputs, int Outputs>
void UnscentedKalmanFilter<States, Inputs, Outputs>::Predict(
    const InputVector& u, units::second_t dt) {
  m_dt = dt;

  // Discretize Q before projecting mean and covariance forward
  StateMatrix contA =
      NumericalJacobianX<States, States, Inputs>(m_f, m_xHat, u);
  StateMatrix discA;
  StateMatrix discQ;
  DiscretizeAQ<States>(contA, m_contQ, m_dt, &discA, &discQ);
  Eigen::internal::llt_inplace<double, Eigen::Lower>::blocked(discQ);

  Matrixd<States, 2 * States + 1> sigmas =
      m_pts.SquareRootSigmaPoints(m_xHat, m_S);

  for (int i = 0; i < m_pts.NumSigmas(); ++i) {
    StateVector x = sigmas.template block<States, 1>(0, i);
    m_sigmasF.template block<States, 1>(0, i) = RK4(m_f, x, u, dt);
  }

  auto [xHat, S] = SquareRootUnscentedTransform<States, States>(
      m_sigmasF, m_pts.Wm(), m_pts.Wc(), m_meanFuncX, m_residualFuncX,
      discQ.template triangularView<Eigen::Lower>());
  m_xHat = xHat;
  m_S = S;
}

template <int States, int Inputs, int Outputs>
template <int Rows>
void UnscentedKalmanFilter<States, Inputs, Outputs>::Correct(
    const InputVector& u, const Vectord<Rows>& y,
    std::function<Vectord<Rows>(const StateVector&, const InputVector&)> h,
    const Matrixd<Rows, Rows>& R) {
  auto meanFuncY = [](auto sigmas, auto Wc) -> Vectord<Rows> {
    return sigmas * Wc;
  };
  auto residualFuncX = [](auto a, auto b) -> StateVector { return a - b; };
  auto residualFuncY = [](auto a, auto b) -> Vectord<Rows> { return a - b; };
  auto addFuncX = [](auto a, auto b) -> StateVector { return a + b; };
  Correct<Rows>(u, y, h, R, meanFuncY, residualFuncY, residualFuncX, addFuncX);
}

template <int States, int Inputs, int Outputs>
template <int Rows>
void UnscentedKalmanFilter<States, Inputs, Outputs>::Correct(
    const InputVector& u, const Vectord<Rows>& y,
    std::function<Vectord<Rows>(const StateVector&, const InputVector&)> h,
    const Matrixd<Rows, Rows>& R,
    std::function<Vectord<Rows>(const Matrixd<Rows, 2 * States + 1>&,
                                const Vectord<2 * States + 1>&)>
        meanFuncY,
    std::function<Vectord<Rows>(const Vectord<Rows>&, const Vectord<Rows>&)>
        residualFuncY,
    std::function<StateVector(const StateVector&, const StateVector&)>
        residualFuncX,
    std::function<StateVector(const StateVector&, const StateVector&)>
        addFuncX) {
  Matrixd<Rows, Rows> discR = DiscretizeR<Rows>(R, m_dt);
  Eigen::internal::llt_inplace<double, Eigen::Lower>::blocked(discR);

  // Transform sigma points into measurement space
  Matrixd<Rows, 2 * States + 1> sigmasH;
  Matrixd<States, 2 * States + 1> sigmas =
      m_pts.SquareRootSigmaPoints(m_xHat, m_S);
  for (int i = 0; i < m_pts.NumSigmas(); ++i) {
    sigmasH.template block<Rows, 1>(0, i) =
        h(sigmas.template block<States, 1>(0, i), u);
  }

  // Mean and covariance of prediction passed through UT
  auto [yHat, Sy] = SquareRootUnscentedTransform<Rows, States>(
      sigmasH, m_pts.Wm(), m_pts.Wc(), meanFuncY, residualFuncY,
      discR.template triangularView<Eigen::Lower>());

  // Compute cross covariance of the state and the measurements
  Matrixd<States, Rows> Pxy;
  Pxy.setZero();
  for (int i = 0; i < m_pts.NumSigmas(); ++i) {
    // Pxy += (sigmas_f[:, i] - x̂)(sigmas_h[:, i] - ŷ)ᵀ W_c[i]
    Pxy += m_pts.Wc(i) *
           (residualFuncX(m_sigmasF.template block<States, 1>(0, i), m_xHat)) *
           (residualFuncY(sigmasH.template block<Rows, 1>(0, i), yHat))
               .transpose();
  }

  // K = (P_{xy} / S_yᵀ) / S_y
  // K = (S_y \ P_{xy}ᵀ)ᵀ / S_y
  // K = (S_yᵀ \ (S_y \ P_{xy}ᵀ))ᵀ
  Matrixd<States, Rows> K =
      Sy.transpose()
          .fullPivHouseholderQr()
          .solve(Sy.fullPivHouseholderQr().solve(Pxy.transpose()))
          .transpose();

  // x̂ₖ₊₁⁺ = x̂ₖ₊₁⁻ + K(y − ŷ)
  m_xHat = addFuncX(m_xHat, K * residualFuncY(y, yHat));

  Matrixd<States, Rows> U = K * Sy;
  for (int i = 0; i < Rows; i++) {
    Eigen::internal::llt_inplace<double, Eigen::Upper>::rankUpdate(
        m_S, U.template block<States, 1>(0, i), -1);
  }
}

}  // namespace frc
