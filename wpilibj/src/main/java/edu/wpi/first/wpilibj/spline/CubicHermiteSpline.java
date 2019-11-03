/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.spline;

import org.ejml.simple.SimpleMatrix;

public class CubicHermiteSpline extends Spline {
  private static SimpleMatrix hermiteBasis;
  private final SimpleMatrix m_coefficients;

  /**
   * Constructs a cubic hermite spline with the specified control vectors. Each
   * control vector contains info about the location of the point and its first
   * derivative.
   *
   * @param xInitialControlVector The control vector for the initial point in
   *                              the x dimension.
   * @param xFinalControlVector   The control vector for the final point in
   *                              the x dimension.
   * @param yInitialControlVector The control vector for the initial point in
   *                              the y dimension.
   * @param yFinalControlVector   The control vector for the final point in
   *                              the y dimension.
   */
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

  /**
   * Returns the coefficients matrix.
   *
   * @return The coefficients matrix.
   */
  @Override
  protected SimpleMatrix getCoefficients() {
    return m_coefficients;
  }

  /**
   * Returns the hermite basis matrix for cubic hermite spline interpolation.
   *
   * @return The hermite basis matrix for cubic hermite spline interpolation.
   */
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

  /**
   * Returns the control vector for each dimension as a matrix from the
   * user-provided arrays in the constructor.
   *
   * @param initialVector The control vector for the initial point.
   * @param finalVector   The control vector for the final point.
   * @return The control vector matrix for a dimension.
   */
  private SimpleMatrix getControlVectorFromArrays(double[] initialVector, double[] finalVector) {
    if (initialVector.length != 2 || finalVector.length != 2) {
      throw new IllegalArgumentException("Size of vectors must be 2");
    }
    return new SimpleMatrix(4, 1, true, new double[]{
        initialVector[0], initialVector[1],
        finalVector[0], finalVector[1]});
  }
}
