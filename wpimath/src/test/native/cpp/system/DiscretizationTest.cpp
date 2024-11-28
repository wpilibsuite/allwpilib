// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <functional>

#include <Eigen/Eigenvalues>
#include <gtest/gtest.h>

#include "frc/EigenCore.h"
#include "frc/system/Discretization.h"
#include "frc/system/NumericalIntegration.h"

// Check that for a simple second-order system that we can easily analyze
// analytically,
TEST(DiscretizationTest, DiscretizeA) {
  frc::Matrixd<2, 2> contA{{0, 1}, {0, 0}};

  frc::Vectord<2> x0{1, 1};
  frc::Matrixd<2, 2> discA;

  frc::DiscretizeA<2>(contA, 1_s, &discA);
  frc::Vectord<2> x1Discrete = discA * x0;

  // We now have pos = vel = 1 and accel = 0, which should give us:
  frc::Vectord<2> x1Truth{1.0 * x0(0) + 1.0 * x0(1), 0.0 * x0(0) + 1.0 * x0(1)};

  EXPECT_EQ(x1Truth, x1Discrete);
}

// Check that for a simple second-order system that we can easily analyze
// analytically,
TEST(DiscretizationTest, DiscretizeAB) {
  frc::Matrixd<2, 2> contA{{0, 1}, {0, 0}};
  frc::Matrixd<2, 1> contB{0, 1};

  frc::Vectord<2> x0{1, 1};
  frc::Vectord<1> u{1};
  frc::Matrixd<2, 2> discA;
  frc::Matrixd<2, 1> discB;

  frc::DiscretizeAB<2, 1>(contA, contB, 1_s, &discA, &discB);
  frc::Vectord<2> x1Discrete = discA * x0 + discB * u;

  // We now have pos = vel = accel = 1, which should give us:
  frc::Vectord<2> x1Truth{1.0 * x0(0) + 1.0 * x0(1) + 0.5 * u(0),
                          0.0 * x0(0) + 1.0 * x0(1) + 1.0 * u(0)};

  EXPECT_EQ(x1Truth, x1Discrete);
}

//                                               T
// Test that the discrete approximation of Q_d ≈ ∫ e^(Aτ) Q e^(Aᵀτ) dτ
//                                               0
TEST(DiscretizationTest, DiscretizeSlowModelAQ) {
  frc::Matrixd<2, 2> contA{{0, 1}, {0, 0}};
  frc::Matrixd<2, 2> contQ{{1, 0}, {0, 1}};

  constexpr auto dt = 1_s;

  //       T
  // Q_d ≈ ∫ e^(Aτ) Q e^(Aᵀτ) dτ
  //       0
  frc::Matrixd<2, 2> discQIntegrated =
      frc::RKDP<std::function<frc::Matrixd<2, 2>(units::second_t,
                                                 const frc::Matrixd<2, 2>&)>,
                frc::Matrixd<2, 2>>(
          [&](units::second_t t, const frc::Matrixd<2, 2>&) {
            return frc::Matrixd<2, 2>((contA * t.value()).exp() * contQ *
                                      (contA.transpose() * t.value()).exp());
          },
          0_s, frc::Matrixd<2, 2>::Zero(), dt);

  frc::Matrixd<2, 2> discA;
  frc::Matrixd<2, 2> discQ;
  frc::DiscretizeAQ<2>(contA, contQ, dt, &discA, &discQ);

  EXPECT_LT((discQIntegrated - discQ).norm(), 1e-10)
      << "Expected these to be nearly equal:\ndiscQ:\n"
      << discQ << "\ndiscQIntegrated:\n"
      << discQIntegrated;
}

//                                               T
// Test that the discrete approximation of Q_d ≈ ∫ e^(Aτ) Q e^(Aᵀτ) dτ
//                                               0
TEST(DiscretizationTest, DiscretizeFastModelAQ) {
  frc::Matrixd<2, 2> contA{{0, 1}, {0, -1406.29}};
  frc::Matrixd<2, 2> contQ{{0.0025, 0}, {0, 1}};

  constexpr auto dt = 5_ms;

  //       T
  // Q_d = ∫ e^(Aτ) Q e^(Aᵀτ) dτ
  //       0
  frc::Matrixd<2, 2> discQIntegrated =
      frc::RKDP<std::function<frc::Matrixd<2, 2>(units::second_t,
                                                 const frc::Matrixd<2, 2>&)>,
                frc::Matrixd<2, 2>>(
          [&](units::second_t t, const frc::Matrixd<2, 2>&) {
            return frc::Matrixd<2, 2>((contA * t.value()).exp() * contQ *
                                      (contA.transpose() * t.value()).exp());
          },
          0_s, frc::Matrixd<2, 2>::Zero(), dt);

  frc::Matrixd<2, 2> discA;
  frc::Matrixd<2, 2> discQ;
  frc::DiscretizeAQ<2>(contA, contQ, dt, &discA, &discQ);

  EXPECT_LT((discQIntegrated - discQ).norm(), 1e-3)
      << "Expected these to be nearly equal:\ndiscQ:\n"
      << discQ << "\ndiscQIntegrated:\n"
      << discQIntegrated;
}

// Test that DiscretizeR() works
TEST(DiscretizationTest, DiscretizeR) {
  frc::Matrixd<2, 2> contR{{2.0, 0.0}, {0.0, 1.0}};
  frc::Matrixd<2, 2> discRTruth{{4.0, 0.0}, {0.0, 2.0}};

  frc::Matrixd<2, 2> discR = frc::DiscretizeR<2>(contR, 500_ms);

  EXPECT_LT((discRTruth - discR).norm(), 1e-10)
      << "Expected these to be nearly equal:\ndiscR:\n"
      << discR << "\ndiscRTruth:\n"
      << discRTruth;
}
