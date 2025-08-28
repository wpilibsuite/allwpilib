// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <functional>

#include <Eigen/Eigenvalues>
#include <gtest/gtest.h>

#include "wpimath/EigenCore.h"
#include "wpimath/system/Discretization.h"
#include "wpimath/system/NumericalIntegration.h"

// Check that for a simple second-order system that we can easily analyze
// analytically,
TEST(DiscretizationTest, DiscretizeA) {
  wpimath::Matrixd<2, 2> contA{{0, 1}, {0, 0}};

  wpimath::Vectord<2> x0{1, 1};
  wpimath::Matrixd<2, 2> discA;

  wpimath::DiscretizeA<2>(contA, 1_s, &discA);
  wpimath::Vectord<2> x1Discrete = discA * x0;

  // We now have pos = vel = 1 and accel = 0, which should give us:
  wpimath::Vectord<2> x1Truth{1.0 * x0(0) + 1.0 * x0(1),
                              0.0 * x0(0) + 1.0 * x0(1)};

  EXPECT_EQ(x1Truth, x1Discrete);
}

// Check that for a simple second-order system that we can easily analyze
// analytically,
TEST(DiscretizationTest, DiscretizeAB) {
  wpimath::Matrixd<2, 2> contA{{0, 1}, {0, 0}};
  wpimath::Matrixd<2, 1> contB{0, 1};

  wpimath::Vectord<2> x0{1, 1};
  wpimath::Vectord<1> u{1};
  wpimath::Matrixd<2, 2> discA;
  wpimath::Matrixd<2, 1> discB;

  wpimath::DiscretizeAB<2, 1>(contA, contB, 1_s, &discA, &discB);
  wpimath::Vectord<2> x1Discrete = discA * x0 + discB * u;

  // We now have pos = vel = accel = 1, which should give us:
  wpimath::Vectord<2> x1Truth{1.0 * x0(0) + 1.0 * x0(1) + 0.5 * u(0),
                              0.0 * x0(0) + 1.0 * x0(1) + 1.0 * u(0)};

  EXPECT_EQ(x1Truth, x1Discrete);
}

//                                               T
// Test that the discrete approximation of Q_d ≈ ∫ e^(Aτ) Q e^(Aᵀτ) dτ
//                                               0
TEST(DiscretizationTest, DiscretizeSlowModelAQ) {
  wpimath::Matrixd<2, 2> contA{{0, 1}, {0, 0}};
  wpimath::Matrixd<2, 2> contQ{{1, 0}, {0, 1}};

  constexpr auto dt = 1_s;

  //       T
  // Q_d ≈ ∫ e^(Aτ) Q e^(Aᵀτ) dτ
  //       0
  wpimath::Matrixd<2, 2> discQIntegrated =
      wpimath::RKDP<std::function<wpimath::Matrixd<2, 2>(
                        units::second_t, const wpimath::Matrixd<2, 2>&)>,
                    wpimath::Matrixd<2, 2>>(
          [&](units::second_t t, const wpimath::Matrixd<2, 2>&) {
            return wpimath::Matrixd<2, 2>(
                (contA * t.value()).exp() * contQ *
                (contA.transpose() * t.value()).exp());
          },
          0_s, wpimath::Matrixd<2, 2>::Zero(), dt);

  wpimath::Matrixd<2, 2> discA;
  wpimath::Matrixd<2, 2> discQ;
  wpimath::DiscretizeAQ<2>(contA, contQ, dt, &discA, &discQ);

  EXPECT_LT((discQIntegrated - discQ).norm(), 1e-10)
      << "Expected these to be nearly equal:\ndiscQ:\n"
      << discQ << "\ndiscQIntegrated:\n"
      << discQIntegrated;
}

//                                               T
// Test that the discrete approximation of Q_d ≈ ∫ e^(Aτ) Q e^(Aᵀτ) dτ
//                                               0
TEST(DiscretizationTest, DiscretizeFastModelAQ) {
  wpimath::Matrixd<2, 2> contA{{0, 1}, {0, -1406.29}};
  wpimath::Matrixd<2, 2> contQ{{0.0025, 0}, {0, 1}};

  constexpr auto dt = 5_ms;

  //       T
  // Q_d = ∫ e^(Aτ) Q e^(Aᵀτ) dτ
  //       0
  wpimath::Matrixd<2, 2> discQIntegrated =
      wpimath::RKDP<std::function<wpimath::Matrixd<2, 2>(
                        units::second_t, const wpimath::Matrixd<2, 2>&)>,
                    wpimath::Matrixd<2, 2>>(
          [&](units::second_t t, const wpimath::Matrixd<2, 2>&) {
            return wpimath::Matrixd<2, 2>(
                (contA * t.value()).exp() * contQ *
                (contA.transpose() * t.value()).exp());
          },
          0_s, wpimath::Matrixd<2, 2>::Zero(), dt);

  wpimath::Matrixd<2, 2> discA;
  wpimath::Matrixd<2, 2> discQ;
  wpimath::DiscretizeAQ<2>(contA, contQ, dt, &discA, &discQ);

  EXPECT_LT((discQIntegrated - discQ).norm(), 1e-3)
      << "Expected these to be nearly equal:\ndiscQ:\n"
      << discQ << "\ndiscQIntegrated:\n"
      << discQIntegrated;
}

// Test that DiscretizeR() works
TEST(DiscretizationTest, DiscretizeR) {
  wpimath::Matrixd<2, 2> contR{{2.0, 0.0}, {0.0, 1.0}};
  wpimath::Matrixd<2, 2> discRTruth{{4.0, 0.0}, {0.0, 2.0}};

  wpimath::Matrixd<2, 2> discR = wpimath::DiscretizeR<2>(contR, 500_ms);

  EXPECT_LT((discRTruth - discR).norm(), 1e-10)
      << "Expected these to be nearly equal:\ndiscR:\n"
      << discR << "\ndiscRTruth:\n"
      << discRTruth;
}
