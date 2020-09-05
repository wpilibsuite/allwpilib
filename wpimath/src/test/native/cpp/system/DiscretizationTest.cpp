/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <gtest/gtest.h>

#include <functional>

#include "Eigen/Core"
#include "Eigen/Eigenvalues"
#include "frc/system/Discretization.h"
#include "frc/system/RungeKutta.h"

// Check that for a simple second-order system that we can easily analyze
// analytically,
TEST(DiscretizationTest, DiscretizeA) {
  Eigen::Matrix<double, 2, 2> contA;
  contA << 0, 1, 0, 0;

  Eigen::Matrix<double, 2, 1> x0;
  x0 << 1, 1;
  Eigen::Matrix<double, 2, 2> discA;

  frc::DiscretizeA<2>(contA, 1_s, &discA);
  Eigen::Matrix<double, 2, 1> x1Discrete = discA * x0;

  // We now have pos = vel = 1 and accel = 0, which should give us:
  Eigen::Matrix<double, 2, 1> x1Truth;
  x1Truth(1) = x0(1);
  x1Truth(0) = x0(0) + 1.0 * x0(1);

  EXPECT_EQ(x1Truth, x1Discrete);
}

// Check that for a simple second-order system that we can easily analyze
// analytically,
TEST(DiscretizationTest, DiscretizeAB) {
  Eigen::Matrix<double, 2, 2> contA;
  contA << 0, 1, 0, 0;

  Eigen::Matrix<double, 2, 1> contB;
  contB << 0, 1;

  Eigen::Matrix<double, 2, 1> x0;
  x0 << 1, 1;
  Eigen::Matrix<double, 1, 1> u;
  u << 1;
  Eigen::Matrix<double, 2, 2> discA;
  Eigen::Matrix<double, 2, 1> discB;

  frc::DiscretizeAB<2, 1>(contA, contB, 1_s, &discA, &discB);
  Eigen::Matrix<double, 2, 1> x1Discrete = discA * x0 + discB * u;

  // We now have pos = vel = accel = 1, which should give us:
  Eigen::Matrix<double, 2, 1> x1Truth;
  x1Truth(1) = x0(1) + 1.0 * u(0);
  x1Truth(0) = x0(0) + 1.0 * x0(1) + 0.5 * u(0);

  EXPECT_EQ(x1Truth, x1Discrete);
}

// Test that the discrete approximation of Q is roughly equal to
// integral from 0 to dt of e^(A tau) Q e^(A.T tau) dtau
TEST(DiscretizationTest, DiscretizeSlowModelAQ) {
  Eigen::Matrix<double, 2, 2> contA;
  contA << 0, 1, 0, 0;

  Eigen::Matrix<double, 2, 2> contQ;
  contQ << 1, 0, 0, 1;

  constexpr auto dt = 1_s;

  Eigen::Matrix<double, 2, 2> discQIntegrated = frc::RungeKuttaTimeVarying<
      std::function<Eigen::Matrix<double, 2, 2>(
          units::second_t, const Eigen::Matrix<double, 2, 2>&)>,
      Eigen::Matrix<double, 2, 2>>(
      [&](units::second_t t, const Eigen::Matrix<double, 2, 2>&) {
        return Eigen::Matrix<double, 2, 2>(
            (contA * t.to<double>()).exp() * contQ *
            (contA.transpose() * t.to<double>()).exp());
      },
      Eigen::Matrix<double, 2, 2>::Zero(), 0_s, dt);

  Eigen::Matrix<double, 2, 2> discA;
  Eigen::Matrix<double, 2, 2> discQ;
  frc::DiscretizeAQ<2>(contA, contQ, dt, &discA, &discQ);

  EXPECT_LT((discQIntegrated - discQ).norm(), 1e-10)
      << "Expected these to be nearly equal:\ndiscQ:\n"
      << discQ << "\ndiscQIntegrated:\n"
      << discQIntegrated;
}

// Test that the discrete approximation of Q is roughly equal to
// integral from 0 to dt of e^(A tau) Q e^(A.T tau) dtau
TEST(DiscretizationTest, DiscretizeFastModelAQ) {
  Eigen::Matrix<double, 2, 2> contA;
  contA << 0, 1, 0, -1406.29;

  Eigen::Matrix<double, 2, 2> contQ;
  contQ << 0.0025, 0, 0, 1;

  constexpr auto dt = 5.05_ms;

  Eigen::Matrix<double, 2, 2> discQIntegrated = frc::RungeKuttaTimeVarying<
      std::function<Eigen::Matrix<double, 2, 2>(
          units::second_t, const Eigen::Matrix<double, 2, 2>&)>,
      Eigen::Matrix<double, 2, 2>>(
      [&](units::second_t t, const Eigen::Matrix<double, 2, 2>&) {
        return Eigen::Matrix<double, 2, 2>(
            (contA * t.to<double>()).exp() * contQ *
            (contA.transpose() * t.to<double>()).exp());
      },
      Eigen::Matrix<double, 2, 2>::Zero(), 0_s, dt);

  Eigen::Matrix<double, 2, 2> discA;
  Eigen::Matrix<double, 2, 2> discQ;
  frc::DiscretizeAQ<2>(contA, contQ, dt, &discA, &discQ);

  EXPECT_LT((discQIntegrated - discQ).norm(), 1e-3)
      << "Expected these to be nearly equal:\ndiscQ:\n"
      << discQ << "\ndiscQIntegrated:\n"
      << discQIntegrated;
}

// Test that the Taylor series discretization produces nearly identical results.
TEST(DiscretizationTest, DiscretizeSlowModelAQTaylor) {
  Eigen::Matrix<double, 2, 2> contA;
  contA << 0, 1, 0, 0;

  Eigen::Matrix<double, 2, 1> contB;
  contB << 0, 1;

  Eigen::Matrix<double, 2, 2> contQ;
  contQ << 1, 0, 0, 1;

  constexpr auto dt = 1_s;

  Eigen::Matrix<double, 2, 2> discQTaylor;
  Eigen::Matrix<double, 2, 2> discA;
  Eigen::Matrix<double, 2, 2> discATaylor;
  Eigen::Matrix<double, 2, 1> discB;

  // Continuous Q should be positive semidefinite
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> esCont(contQ);
  for (int i = 0; i < contQ.rows(); i++) {
    EXPECT_GT(esCont.eigenvalues()[i], 0);
  }

  Eigen::Matrix<double, 2, 2> discQIntegrated = frc::RungeKuttaTimeVarying<
      std::function<Eigen::Matrix<double, 2, 2>(
          units::second_t, const Eigen::Matrix<double, 2, 2>&)>,
      Eigen::Matrix<double, 2, 2>>(
      [&](units::second_t t, const Eigen::Matrix<double, 2, 2>&) {
        return Eigen::Matrix<double, 2, 2>(
            (contA * t.to<double>()).exp() * contQ *
            (contA.transpose() * t.to<double>()).exp());
      },
      Eigen::Matrix<double, 2, 2>::Zero(), 0_s, dt);

  frc::DiscretizeAB<2, 1>(contA, contB, dt, &discA, &discB);
  frc::DiscretizeAQTaylor<2>(contA, contQ, dt, &discATaylor, &discQTaylor);

  EXPECT_LT((discQIntegrated - discQTaylor).norm(), 1e-10)
      << "Expected these to be nearly equal:\ndiscQTaylor:\n"
      << discQTaylor << "\ndiscQIntegrated:\n"
      << discQIntegrated;
  EXPECT_LT((discA - discATaylor).norm(), 1e-10);

  // Discrete Q should be positive semidefinite
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> esDisc(discQTaylor);
  for (int i = 0; i < discQTaylor.rows(); i++) {
    EXPECT_GT(esDisc.eigenvalues()[i], 0);
  }
}

// Test that the Taylor series discretization produces nearly identical results.
TEST(DiscretizationTest, DiscretizeFastModelAQTaylor) {
  Eigen::Matrix<double, 2, 2> contA;
  contA << 0, 1, 0, -1500;

  Eigen::Matrix<double, 2, 1> contB;
  contB << 0, 1;

  Eigen::Matrix<double, 2, 2> contQ;
  contQ << 0.0025, 0, 0, 1;

  constexpr auto dt = 5.05_ms;

  Eigen::Matrix<double, 2, 2> discQTaylor;
  Eigen::Matrix<double, 2, 2> discA;
  Eigen::Matrix<double, 2, 2> discATaylor;
  Eigen::Matrix<double, 2, 1> discB;

  // Continuous Q should be positive semidefinite
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> esCont(contQ);
  for (int i = 0; i < contQ.rows(); i++) {
    EXPECT_GT(esCont.eigenvalues()[i], 0);
  }

  Eigen::Matrix<double, 2, 2> discQIntegrated = frc::RungeKuttaTimeVarying<
      std::function<Eigen::Matrix<double, 2, 2>(
          units::second_t, const Eigen::Matrix<double, 2, 2>&)>,
      Eigen::Matrix<double, 2, 2>>(
      [&](units::second_t t, const Eigen::Matrix<double, 2, 2>&) {
        return Eigen::Matrix<double, 2, 2>(
            (contA * t.to<double>()).exp() * contQ *
            (contA.transpose() * t.to<double>()).exp());
      },
      Eigen::Matrix<double, 2, 2>::Zero(), 0_s, dt);

  frc::DiscretizeAB<2, 1>(contA, contB, dt, &discA, &discB);
  frc::DiscretizeAQTaylor<2>(contA, contQ, dt, &discATaylor, &discQTaylor);

  EXPECT_LT((discQIntegrated - discQTaylor).norm(), 1e-3)
      << "Expected these to be nearly equal:\ndiscQTaylor:\n"
      << discQTaylor << "\ndiscQIntegrated:\n"
      << discQIntegrated;
  EXPECT_LT((discA - discATaylor).norm(), 1e-10);

  // Discrete Q should be positive semidefinite
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> esDisc(discQTaylor);
  for (int i = 0; i < discQTaylor.rows(); i++) {
    EXPECT_GT(esDisc.eigenvalues()[i], 0);
  }
}

// Test that DiscretizeR() works
TEST(DiscretizationTest, DiscretizeR) {
  Eigen::Matrix<double, 2, 2> contR;
  contR << 2.0, 0.0, 0.0, 1.0;

  Eigen::Matrix<double, 2, 2> discRTruth;
  discRTruth << 4.0, 0.0, 0.0, 2.0;

  Eigen::Matrix<double, 2, 2> discR = frc::DiscretizeR<2>(contR, 500_ms);

  EXPECT_LT((discRTruth - discR).norm(), 1e-10)
      << "Expected these to be nearly equal:\ndiscR:\n"
      << discR << "\ndiscRTruth:\n"
      << discRTruth;
}
