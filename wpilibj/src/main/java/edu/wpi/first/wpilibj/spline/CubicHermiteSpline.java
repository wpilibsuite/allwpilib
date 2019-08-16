/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.spline;

import org.ejml.simple.SimpleMatrix;

public class CubicHermiteSpline extends Spline {
  private static SimpleMatrix hermiteBasis = null;
  private SimpleMatrix m_coefficients;

  @SuppressWarnings("ParameterName")
  public CubicHermiteSpline(double[] xInitialControlVector, double[] xFinalControlVector,
                            double[] yInitialControlVector, double[] yFinalControlVector) {
    super(3);

    // Populate the coefficients for the actual spline equations.
    // Row 0 is x coefficients
    // Row 1 is y coefficients
    final var hermite = makeHermiteBasis();
    final var x = getControlVectorFromArrays(xInitialControlVector, xFinalControlVector);
    final var y = getControlVectorFromArrays(yInitialControlVector, yFinalControlVector);

    final var xCoeffs = (hermite.mult(x)).transpose();
    final var yCoeffs = (hermite.mult(y)).transpose();

    m_coefficients = new SimpleMatrix(6, 4);

    for (int i = 0; i < 4; i++) {
      m_coefficients.set(0, i, xCoeffs.get(0, i));
      m_coefficients.set(1, i, yCoeffs.get(0, i));

      // Populate Row 2 and Row 3 with the derivatives of the equations above.
      // Then populate row 4 and 5 with the second derivatives.
      m_coefficients.set(2, i, m_coefficients.get(0, i) * (3 - i));
      m_coefficients.set(3, i, m_coefficients.get(1, i) * (3 - i));
      m_coefficients.set(4, i, m_coefficients.get(2, i) * (3 - i));
      m_coefficients.set(5, i, m_coefficients.get(3, i) * (3 - i));
    }

  }

  @Override
  protected SimpleMatrix getCoefficients() {
    return m_coefficients;
  }

  private SimpleMatrix makeHermiteBasis() {
    if (hermiteBasis == null) {
      hermiteBasis = new SimpleMatrix(4, 4, true, new double[]{
          +2.0, +1.0, -2.0, +1.0,
          -3.0, -2.0, +3.0, -1.0,
          +0.0, +1.0, +0.0, +0.0,
          +1.0, +0.0, +0.0, +0.0
      });
    }
    return hermiteBasis;
  }

  private SimpleMatrix getControlVectorFromArrays(double[] initialVector, double[] finalVector) {
    if (initialVector.length != 2 || finalVector.length != 2) {
      throw new IllegalArgumentException("Size of vectors must be 2");
    }
    return new SimpleMatrix(4, 1, true, new double[]{
        initialVector[0], initialVector[1],
        finalVector[0], finalVector[1]});
  }
}
