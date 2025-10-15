// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <functional>

#include <Eigen/Eigenvalues>
#include <gtest/gtest.h>

#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/math/system/Discretization.hpp"
#include "wpi/math/system/NumericalIntegration.hpp"

// Check that for a simple second-order system that we can easily analyze
// analytically,
TEST(DiscretizationTest, DiscretizeA) {
  wpi::math::Matrixd<2, 2> contA{{0, 1}, {0, 0}};

  wpi::math::Vectord<2> x0{1, 1};
  wpi::math::Matrixd<2, 2> discA;

  wpi::math::DiscretizeA<2>(contA, 1_s, &discA);
  wpi::math::Vectord<2> x1Discrete = discA * x0;

  // We now have pos = vel = 1 and accel = 0, which should give us:
  wpi::math::Vectord<2> x1Truth{1.0 * x0(0) + 1.0 * x0(1), 0.0 * x0(0) + 1.0 * x0(1)};

  EXPECT_EQ(x1Truth, x1Discrete);
}

// Check that for a simple second-order system that we can easily analyze
// analytically,
TEST(DiscretizationTest, DiscretizeAB) {
  wpi::math::Matrixd<2, 2> contA{{0, 1}, {0, 0}};
  wpi::math::Matrixd<2, 1> contB{0, 1};

  wpi::math::Vectord<2> x0{1, 1};
  wpi::math::Vectord<1> u{1};
  wpi::math::Matrixd<2, 2> discA;
  wpi::math::Matrixd<2, 1> discB;

  wpi::math::DiscretizeAB<2, 1>(contA, contB, 1_s, &discA, &discB);
  wpi::math::Vectord<2> x1Discrete = discA * x0 + discB * u;

  // We now have pos = vel = accel = 1, which should give us:
  wpi::math::Vectord<2> x1Truth{1.0 * x0(0) + 1.0 * x0(1) + 0.5 * u(0),
                          0.0 * x0(0) + 1.0 * x0(1) + 1.0 * u(0)};

  EXPECT_EQ(x1Truth, x1Discrete);
}

//                                               T
// Test that the discrete approximation of Q_d ≈ ∫ e^(Aτ) Q e^(Aᵀτ) dτ
//                                               0
TEST(DiscretizationTest, DiscretizeSlowModelAQ) {
  wpi::math::Matrixd<2, 2> contA{{0, 1}, {0, 0}};
  wpi::math::Matrixd<2, 2> contQ{{1, 0}, {0, 1}};

  constexpr wpi::units::second_t dt = 1_s;

  //       T
  // Q_d ≈ ∫ e^(Aτ) Q e^(Aᵀτ) dτ
  //       0
  wpi::math::Matrixd<2, 2> discQIntegrated =
      wpi::math::RKDP<std::function<wpi::math::Matrixd<2, 2>(wpi::units::second_t,
                                                 const wpi::math::Matrixd<2, 2>&)>,
                wpi::math::Matrixd<2, 2>>(
          [&](wpi::units::second_t t, const wpi::math::Matrixd<2, 2>&) {
            return wpi::math::Matrixd<2, 2>((contA * t.value()).exp() * contQ *
                                      (contA.transpose() * t.value()).exp());
          },
          0_s, wpi::math::Matrixd<2, 2>::Zero(), dt);

  wpi::math::Matrixd<2, 2> discA;
  wpi::math::Matrixd<2, 2> discQ;
  wpi::math::DiscretizeAQ<2>(contA, contQ, dt, &discA, &discQ);

  EXPECT_LT((discQIntegrated - discQ).norm(), 1e-10)
      << "Expected these to be nearly equal:\ndiscQ:\n"
      << discQ << "\ndiscQIntegrated:\n"
      << discQIntegrated;
}

//                                               T
// Test that the discrete approximation of Q_d ≈ ∫ e^(Aτ) Q e^(Aᵀτ) dτ
//                                               0
TEST(DiscretizationTest, DiscretizeFastModelAQ) {
  wpi::math::Matrixd<2, 2> contA{{0, 1}, {0, -1406.29}};
  wpi::math::Matrixd<2, 2> contQ{{0.0025, 0}, {0, 1}};

  constexpr wpi::units::second_t dt = 5_ms;

  //       T
  // Q_d = ∫ e^(Aτ) Q e^(Aᵀτ) dτ
  //       0
  wpi::math::Matrixd<2, 2> discQIntegrated =
      wpi::math::RKDP<std::function<wpi::math::Matrixd<2, 2>(wpi::units::second_t,
                                                 const wpi::math::Matrixd<2, 2>&)>,
                wpi::math::Matrixd<2, 2>>(
          [&](wpi::units::second_t t, const wpi::math::Matrixd<2, 2>&) {
            return wpi::math::Matrixd<2, 2>((contA * t.value()).exp() * contQ *
                                      (contA.transpose() * t.value()).exp());
          },
          0_s, wpi::math::Matrixd<2, 2>::Zero(), dt);

  wpi::math::Matrixd<2, 2> discA;
  wpi::math::Matrixd<2, 2> discQ;
  wpi::math::DiscretizeAQ<2>(contA, contQ, dt, &discA, &discQ);

  EXPECT_LT((discQIntegrated - discQ).norm(), 1e-3)
      << "Expected these to be nearly equal:\ndiscQ:\n"
      << discQ << "\ndiscQIntegrated:\n"
      << discQIntegrated;
}

// Test that DiscretizeR() works
TEST(DiscretizationTest, DiscretizeR) {
  wpi::math::Matrixd<2, 2> contR{{2.0, 0.0}, {0.0, 1.0}};
  wpi::math::Matrixd<2, 2> discRTruth{{4.0, 0.0}, {0.0, 2.0}};

  wpi::math::Matrixd<2, 2> discR = wpi::math::DiscretizeR<2>(contR, 500_ms);

  EXPECT_LT((discRTruth - discR).norm(), 1e-10)
      << "Expected these to be nearly equal:\ndiscR:\n"
      << discR << "\ndiscRTruth:\n"
      << discRTruth;
}
