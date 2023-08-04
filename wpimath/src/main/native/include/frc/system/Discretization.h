// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/EigenCore.h"
#include "units/time.h"
#include "unsupported/Eigen/MatrixFunctions"

namespace frc {

/**
 * Discretizes the given continuous A matrix.
 *
 * @tparam States Number of states.
 * @param contA Continuous system matrix.
 * @param dt    Discretization timestep.
 * @param discA Storage for discrete system matrix.
 */
template <int States>
void DiscretizeA(const Matrixd<States, States>& contA, units::second_t dt,
                 Matrixd<States, States>* discA) {
  // A_d = eᴬᵀ
  *discA = (contA * dt.value()).exp();
}

/**
 * Discretizes the given continuous A and B matrices.
 *
 * @tparam States Number of states.
 * @tparam Inputs Number of inputs.
 * @param contA Continuous system matrix.
 * @param contB Continuous input matrix.
 * @param dt    Discretization timestep.
 * @param discA Storage for discrete system matrix.
 * @param discB Storage for discrete input matrix.
 */
template <int States, int Inputs>
void DiscretizeAB(const Matrixd<States, States>& contA,
                  const Matrixd<States, Inputs>& contB, units::second_t dt,
                  Matrixd<States, States>* discA,
                  Matrixd<States, Inputs>* discB) {
  // M = [A  B]
  //     [0  0]
  Matrixd<States + Inputs, States + Inputs> M;
  M.template block<States, States>(0, 0) = contA;
  M.template block<States, Inputs>(0, States) = contB;
  M.template block<Inputs, States + Inputs>(States, 0).setZero();

  // ϕ = eᴹᵀ = [A_d  B_d]
  //           [ 0    I ]
  Matrixd<States + Inputs, States + Inputs> phi = (M * dt.value()).exp();

  *discA = phi.template block<States, States>(0, 0);
  *discB = phi.template block<States, Inputs>(0, States);
}

/**
 * Discretizes the given continuous A and Q matrices.
 *
 * @tparam States Number of states.
 * @param contA Continuous system matrix.
 * @param contQ Continuous process noise covariance matrix.
 * @param dt    Discretization timestep.
 * @param discA Storage for discrete system matrix.
 * @param discQ Storage for discrete process noise covariance matrix.
 */
template <int States>
void DiscretizeAQ(const Matrixd<States, States>& contA,
                  const Matrixd<States, States>& contQ, units::second_t dt,
                  Matrixd<States, States>* discA,
                  Matrixd<States, States>* discQ) {
  // Make continuous Q symmetric if it isn't already
  Matrixd<States, States> Q = (contQ + contQ.transpose()) / 2.0;

  // M = [−A  Q ]
  //     [ 0  Aᵀ]
  Matrixd<2 * States, 2 * States> M;
  M.template block<States, States>(0, 0) = -contA;
  M.template block<States, States>(0, States) = Q;
  M.template block<States, States>(States, 0).setZero();
  M.template block<States, States>(States, States) = contA.transpose();

  // ϕ = eᴹᵀ = [−A_d  A_d⁻¹Q_d]
  //           [ 0      A_dᵀ  ]
  Matrixd<2 * States, 2 * States> phi = (M * dt.value()).exp();

  // ϕ₁₂ = A_d⁻¹Q_d
  Matrixd<States, States> phi12 = phi.block(0, States, States, States);

  // ϕ₂₂ = A_dᵀ
  Matrixd<States, States> phi22 = phi.block(States, States, States, States);

  *discA = phi22.transpose();

  Q = *discA * phi12;

  // Make discrete Q symmetric if it isn't already
  *discQ = (Q + Q.transpose()) / 2.0;
}

/**
 * Discretizes the given continuous A and Q matrices.
 *
 * Rather than solving a 2N x 2N matrix exponential like in DiscretizeAQ()
 * (which is expensive), we take advantage of the structure of the block matrix
 * of A and Q.
 *
 * <ul>
 *   <li>eᴬᵀ, which is only N x N, is relatively cheap.
 *   <li>The upper-right quarter of the 2N x 2N matrix, which we can approximate
 *       using a taylor series to several terms and still be substantially
 *       cheaper than taking the big exponential.
 * </ul>
 *
 * @tparam States Number of states.
 * @param contA Continuous system matrix.
 * @param contQ Continuous process noise covariance matrix.
 * @param dt    Discretization timestep.
 * @param discA Storage for discrete system matrix.
 * @param discQ Storage for discrete process noise covariance matrix.
 */
template <int States>
void DiscretizeAQTaylor(const Matrixd<States, States>& contA,
                        const Matrixd<States, States>& contQ,
                        units::second_t dt, Matrixd<States, States>* discA,
                        Matrixd<States, States>* discQ) {
  //       T
  // Q_d = ∫ e^(Aτ) Q e^(Aᵀτ) dτ
  //       0
  //
  // M = [−A  Q ]
  //     [ 0  Aᵀ]
  // ϕ = eᴹᵀ
  // ϕ₁₂ = A_d⁻¹Q_d
  //
  // Taylor series of ϕ:
  //
  //   ϕ = eᴹᵀ = I + MT + 1/2 M²T² + 1/6 M³T³ + …
  //   ϕ = eᴹᵀ = I + MT + 1/2 T²M² + 1/6 T³M³ + …
  //
  // Taylor series of ϕ expanded for ϕ₁₂:
  //
  //   ϕ₁₂ = 0 + QT + 1/2 T² (−AQ + QAᵀ) + 1/6 T³ (−A lastTerm + Q Aᵀ²) + …
  //
  // ```
  // lastTerm = Q
  // lastCoeff = T
  // ATn = Aᵀ
  // ϕ₁₂ = lastTerm lastCoeff = QT
  //
  // for i in range(2, 6):
  //   // i = 2
  //   lastTerm = −A lastTerm + Q ATn = −AQ + QAᵀ
  //   lastCoeff *= T/i → lastCoeff *= T/2 = 1/2 T²
  //   ATn *= Aᵀ = Aᵀ²
  //
  //   // i = 3
  //   lastTerm = −A lastTerm + Q ATn = −A (−AQ + QAᵀ) + QAᵀ² = …
  //   …
  // ```

  // Make continuous Q symmetric if it isn't already
  Matrixd<States, States> Q = (contQ + contQ.transpose()) / 2.0;

  Matrixd<States, States> lastTerm = Q;
  double lastCoeff = dt.value();

  // Aᵀⁿ
  Matrixd<States, States> ATn = contA.transpose();

  Matrixd<States, States> phi12 = lastTerm * lastCoeff;

  // i = 6 i.e. 5th order should be enough precision
  for (int i = 2; i < 6; ++i) {
    lastTerm = -contA * lastTerm + Q * ATn;
    lastCoeff *= dt.value() / static_cast<double>(i);

    phi12 += lastTerm * lastCoeff;

    ATn *= contA.transpose();
  }

  DiscretizeA<States>(contA, dt, discA);
  Q = *discA * phi12;

  // Make discrete Q symmetric if it isn't already
  *discQ = (Q + Q.transpose()) / 2.0;
}

/**
 * Returns a discretized version of the provided continuous measurement noise
 * covariance matrix.
 *
 * @tparam Outputs Number of outputs.
 * @param R  Continuous measurement noise covariance matrix.
 * @param dt Discretization timestep.
 */
template <int Outputs>
Matrixd<Outputs, Outputs> DiscretizeR(const Matrixd<Outputs, Outputs>& R,
                                      units::second_t dt) {
  // R_d = 1/T R
  return R / dt.value();
}

}  // namespace frc
