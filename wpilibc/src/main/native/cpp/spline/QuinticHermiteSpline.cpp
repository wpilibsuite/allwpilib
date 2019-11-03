/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/spline/QuinticHermiteSpline.h"

using namespace frc;

QuinticHermiteSpline::QuinticHermiteSpline(
    std::array<double, 3> xInitialControlVector,
    std::array<double, 3> xFinalControlVector,
    std::array<double, 3> yInitialControlVector,
    std::array<double, 3> yFinalControlVector) {
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
    m_coefficients.template block<2, 1>(2, i) =
        m_coefficients.template block<2, 1>(0, i) * (5 - i);

    m_coefficients.template block<2, 1>(4, i) =
        m_coefficients.template block<2, 1>(2, i) * (5 - i);
  }
}
