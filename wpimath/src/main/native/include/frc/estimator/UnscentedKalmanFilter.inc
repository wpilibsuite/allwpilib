// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Eigen/Cholesky"
#include "frc/StateSpaceUtil.h"
#include "frc/estimator/UnscentedKalmanFilter.h"
#include "frc/estimator/UnscentedTransform.h"
#include "frc/system/Discretization.h"
#include "frc/system/NumericalIntegration.h"
#include "frc/system/NumericalJacobian.h"

namespace frc {

template <int States, int Inputs, int Outputs>
UnscentedKalmanFilter<States, Inputs, Outputs>::UnscentedKalmanFilter(
    std::function<
        Eigen::Vector<double, States>(const Eigen::Vector<double, States>&,
                                      const Eigen::Vector<double, Inputs>&)>
        f,
    std::function<
        Eigen::Vector<double, Outputs>(const Eigen::Vector<double, States>&,
                                       const Eigen::Vector<double, Inputs>&)>
        h,
    const wpi::array<double, States>& stateStdDevs,
    const wpi::array<double, Outputs>& measurementStdDevs, units::second_t dt)
    : m_f(f), m_h(h) {
  m_contQ = MakeCovMatrix(stateStdDevs);
  m_contR = MakeCovMatrix(measurementStdDevs);
  m_meanFuncX = [](auto sigmas, auto Wm) -> Eigen::Vector<double, States> {
    return sigmas * Wm;
  };
  m_meanFuncY = [](auto sigmas, auto Wc) -> Eigen::Vector<double, Outputs> {
    return sigmas * Wc;
  };
  m_residualFuncX = [](auto a, auto b) -> Eigen::Vector<double, States> {
    return a - b;
  };
  m_residualFuncY = [](auto a, auto b) -> Eigen::Vector<double, Outputs> {
    return a - b;
  };
  m_addFuncX = [](auto a, auto b) -> Eigen::Vector<double, States> {
    return a + b;
  };
  m_dt = dt;

  Reset();
}

template <int States, int Inputs, int Outputs>
UnscentedKalmanFilter<States, Inputs, Outputs>::UnscentedKalmanFilter(
    std::function<
        Eigen::Vector<double, States>(const Eigen::Vector<double, States>&,
                                      const Eigen::Vector<double, Inputs>&)>
        f,
    std::function<
        Eigen::Vector<double, Outputs>(const Eigen::Vector<double, States>&,
                                       const Eigen::Vector<double, Inputs>&)>
        h,
    const wpi::array<double, States>& stateStdDevs,
    const wpi::array<double, Outputs>& measurementStdDevs,
    std::function<Eigen::Vector<double, States>(
        const Eigen::Matrix<double, States, 2 * States + 1>&,
        const Eigen::Vector<double, 2 * States + 1>&)>
        meanFuncX,
    std::function<Eigen::Vector<double, Outputs>(
        const Eigen::Matrix<double, Outputs, 2 * States + 1>&,
        const Eigen::Vector<double, 2 * States + 1>&)>
        meanFuncY,
    std::function<
        Eigen::Vector<double, States>(const Eigen::Vector<double, States>&,
                                      const Eigen::Vector<double, States>&)>
        residualFuncX,
    std::function<
        Eigen::Vector<double, Outputs>(const Eigen::Vector<double, Outputs>&,
                                       const Eigen::Vector<double, Outputs>&)>
        residualFuncY,
    std::function<
        Eigen::Vector<double, States>(const Eigen::Vector<double, States>&,
                                      const Eigen::Vector<double, States>&)>
        addFuncX,
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
    const Eigen::Vector<double, Inputs>& u, units::second_t dt) {
  m_dt = dt;

  // Discretize Q before projecting mean and covariance forward
  Eigen::Matrix<double, States, States> contA =
      NumericalJacobianX<States, States, Inputs>(m_f, m_xHat, u);
  Eigen::Matrix<double, States, States> discA;
  Eigen::Matrix<double, States, States> discQ;
  DiscretizeAQTaylor<States>(contA, m_contQ, dt, &discA, &discQ);

  Eigen::Matrix<double, States, 2 * States + 1> sigmas =
      m_pts.SigmaPoints(m_xHat, m_P);

  for (int i = 0; i < m_pts.NumSigmas(); ++i) {
    Eigen::Vector<double, States> x = sigmas.template block<States, 1>(0, i);
    m_sigmasF.template block<States, 1>(0, i) = RK4(m_f, x, u, dt);
  }

  auto ret = UnscentedTransform<States, States>(
      m_sigmasF, m_pts.Wm(), m_pts.Wc(), m_meanFuncX, m_residualFuncX);
  m_xHat = std::get<0>(ret);
  m_P = std::get<1>(ret);

  m_P += discQ;
}

template <int States, int Inputs, int Outputs>
template <int Rows>
void UnscentedKalmanFilter<States, Inputs, Outputs>::Correct(
    const Eigen::Vector<double, Inputs>& u,
    const Eigen::Vector<double, Rows>& y,
    std::function<
        Eigen::Vector<double, Rows>(const Eigen::Vector<double, States>&,
                                    const Eigen::Vector<double, Inputs>&)>
        h,
    const Eigen::Matrix<double, Rows, Rows>& R) {
  auto meanFuncY = [](auto sigmas, auto Wc) -> Eigen::Vector<double, Rows> {
    return sigmas * Wc;
  };
  auto residualFuncX = [](auto a, auto b) -> Eigen::Vector<double, States> {
    return a - b;
  };
  auto residualFuncY = [](auto a, auto b) -> Eigen::Vector<double, Rows> {
    return a - b;
  };
  auto addFuncX = [](auto a, auto b) -> Eigen::Vector<double, States> {
    return a + b;
  };
  Correct<Rows>(u, y, h, R, meanFuncY, residualFuncY, residualFuncX, addFuncX);
}

template <int States, int Inputs, int Outputs>
template <int Rows>
void UnscentedKalmanFilter<States, Inputs, Outputs>::Correct(
    const Eigen::Vector<double, Inputs>& u,
    const Eigen::Vector<double, Rows>& y,
    std::function<
        Eigen::Vector<double, Rows>(const Eigen::Vector<double, States>&,
                                    const Eigen::Vector<double, Inputs>&)>
        h,
    const Eigen::Matrix<double, Rows, Rows>& R,
    std::function<Eigen::Vector<double, Rows>(
        const Eigen::Matrix<double, Rows, 2 * States + 1>&,
        const Eigen::Vector<double, 2 * States + 1>&)>
        meanFuncY,
    std::function<Eigen::Vector<double, Rows>(
        const Eigen::Vector<double, Rows>&, const Eigen::Vector<double, Rows>&)>
        residualFuncY,
    std::function<
        Eigen::Vector<double, States>(const Eigen::Vector<double, States>&,
                                      const Eigen::Vector<double, States>&)>
        residualFuncX,
    std::function<
        Eigen::Vector<double, States>(const Eigen::Vector<double, States>&,
                                      const Eigen::Vector<double, States>)>
        addFuncX) {
  const Eigen::Matrix<double, Rows, Rows> discR = DiscretizeR<Rows>(R, m_dt);

  // Transform sigma points into measurement space
  Eigen::Matrix<double, Rows, 2 * States + 1> sigmasH;
  Eigen::Matrix<double, States, 2 * States + 1> sigmas =
      m_pts.SigmaPoints(m_xHat, m_P);
  for (int i = 0; i < m_pts.NumSigmas(); ++i) {
    sigmasH.template block<Rows, 1>(0, i) =
        h(sigmas.template block<States, 1>(0, i), u);
  }

  // Mean and covariance of prediction passed through UT
  auto [yHat, Py] = UnscentedTransform<Rows, States>(
      sigmasH, m_pts.Wm(), m_pts.Wc(), meanFuncY, residualFuncY);
  Py += discR;

  // Compute cross covariance of the state and the measurements
  Eigen::Matrix<double, States, Rows> Pxy;
  Pxy.setZero();
  for (int i = 0; i < m_pts.NumSigmas(); ++i) {
    // Pxy += (sigmas_f[:, i] - x̂)(sigmas_h[:, i] - ŷ)ᵀ W_c[i]
    Pxy += m_pts.Wc(i) *
           (residualFuncX(m_sigmasF.template block<States, 1>(0, i), m_xHat)) *
           (residualFuncY(sigmasH.template block<Rows, 1>(0, i), yHat))
               .transpose();
  }

  // K = P_{xy} P_y⁻¹
  // Kᵀ = P_yᵀ⁻¹ P_{xy}ᵀ
  // P_yᵀKᵀ = P_{xy}ᵀ
  // Kᵀ = P_yᵀ.solve(P_{xy}ᵀ)
  // K = (P_yᵀ.solve(P_{xy}ᵀ)ᵀ
  Eigen::Matrix<double, States, Rows> K =
      Py.transpose().ldlt().solve(Pxy.transpose()).transpose();

  // x̂ₖ₊₁⁺ = x̂ₖ₊₁⁻ + K(y − ŷ)
  m_xHat = addFuncX(m_xHat, K * residualFuncY(y, yHat));

  // Pₖ₊₁⁺ = Pₖ₊₁⁻ − KP_yKᵀ
  m_P -= K * Py * K.transpose();
}

}  // namespace frc
