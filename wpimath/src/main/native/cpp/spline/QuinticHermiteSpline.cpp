// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/spline/QuinticHermiteSpline.h"

using namespace frc;

QuinticHermiteSpline::QuinticHermiteSpline(
    wpi::array<double, 3> xInitialControlVector,
    wpi::array<double, 3> xFinalControlVector,
    wpi::array<double, 3> yInitialControlVector,
    wpi::array<double, 3> yFinalControlVector) {
  const auto hermite = MakeHermiteBasis();
  const auto x =
      ControlVectorFromArrays(xInitialControlVector, xFinalControlVector);
  const auto y =
      ControlVectorFromArrays(yInitialControlVector, yFinalControlVector);

  // Populate first two rows with coefficients.
  m_coefficients.template block<1, 6>(0, 0) = (hermite * x).transpose();
  m_coefficients.template block<1, 6>(1, 0) = (hermite * y).transpose();

  // Populate Row 2 and Row 3 with the derivatives of the equations above.
  // Then populate row 4 and 5 with the second derivatives.
  for (int i = 0; i < 6; i++) {
    // Here, we are multiplying by (5 - i) to manually take the derivative. The
    // power of the term in index 0 is 5, index 1 is 4 and so on. To find the
    // coefficient of the derivative, we can use the power rule and multiply
    // the existing coefficient by its power.
    m_coefficients.template block<2, 1>(2, i) =
        m_coefficients.template block<2, 1>(0, i) * (5 - i);
  }
  for (int i = 0; i < 5; i++) {
    // Here, we are multiplying by (4 - i) to manually take the derivative. The
    // power of the term in index 0 is 4, index 1 is 3 and so on. To find the
    // coefficient of the derivative, we can use the power rule and multiply
    // the existing coefficient by its power.
    m_coefficients.template block<2, 1>(4, i) =
        m_coefficients.template block<2, 1>(2, i) * (4 - i);
  }
}
