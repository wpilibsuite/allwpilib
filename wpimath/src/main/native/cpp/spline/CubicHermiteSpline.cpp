// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/spline/CubicHermiteSpline.h"

using namespace frc;

CubicHermiteSpline::CubicHermiteSpline(
    wpi::array<double, 2> xInitialControlVector,
    wpi::array<double, 2> xFinalControlVector,
    wpi::array<double, 2> yInitialControlVector,
    wpi::array<double, 2> yFinalControlVector) {
  const auto hermite = MakeHermiteBasis();
  const auto x =
      ControlVectorFromArrays(xInitialControlVector, xFinalControlVector);
  const auto y =
      ControlVectorFromArrays(yInitialControlVector, yFinalControlVector);

  // Populate first two rows with coefficients.
  m_coefficients.template block<1, 4>(0, 0) = hermite * x;
  m_coefficients.template block<1, 4>(1, 0) = hermite * y;

  // Populate Row 2 and Row 3 with the derivatives of the equations above.
  // Then populate row 4 and 5 with the second derivatives.
  for (int i = 0; i < 4; i++) {
    // Here, we are multiplying by (3 - i) to manually take the derivative. The
    // power of the term in index 0 is 3, index 1 is 2 and so on. To find the
    // coefficient of the derivative, we can use the power rule and multiply
    // the existing coefficient by its power.
    m_coefficients.template block<2, 1>(2, i) =
        m_coefficients.template block<2, 1>(0, i) * (3 - i);
  }

  for (int i = 0; i < 3; i++) {
    // Here, we are multiplying by (2 - i) to manually take the derivative. The
    // power of the term in index 0 is 2, index 1 is 1 and so on. To find the
    // coefficient of the derivative, we can use the power rule and multiply
    // the existing coefficient by its power.
    m_coefficients.template block<2, 1>(4, i) =
        m_coefficients.template block<2, 1>(2, i) * (2 - i);
  }
}
