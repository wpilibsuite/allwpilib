// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.spline;

import edu.wpi.first.math.spline.proto.CubicHermiteSplineProto;
import edu.wpi.first.math.spline.struct.CubicHermiteSplineStruct;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;
import org.ejml.simple.SimpleMatrix;

/** Represents a hermite spline of degree 3. */
public class CubicHermiteSpline extends Spline implements ProtobufSerializable, StructSerializable {
  private static SimpleMatrix hermiteBasis;
  private final SimpleMatrix m_coefficients;

  /** The control vector for the initial point in the x dimension. DO NOT MODIFY THIS ARRAY! */
  public final double[] xInitialControlVector;

  /** The control vector for the final point in the x dimension. DO NOT MODIFY THIS ARRAY! */
  public final double[] xFinalControlVector;

  /** The control vector for the initial point in the y dimension. DO NOT MODIFY THIS ARRAY! */
  public final double[] yInitialControlVector;

  /** The control vector for the final point in the y dimension. DO NOT MODIFY THIS ARRAY! */
  public final double[] yFinalControlVector;

  private final ControlVector m_initialControlVector;
  private final ControlVector m_finalControlVector;

  /**
   * Constructs a cubic hermite spline with the specified control vectors. Each control vector
   * contains info about the location of the point and its first derivative.
   *
   * @param xInitialControlVector The control vector for the initial point in the x dimension.
   * @param xFinalControlVector The control vector for the final point in the x dimension.
   * @param yInitialControlVector The control vector for the initial point in the y dimension.
   * @param yFinalControlVector The control vector for the final point in the y dimension.
   */
  @SuppressWarnings("PMD.ArrayIsStoredDirectly")
  public CubicHermiteSpline(
      double[] xInitialControlVector,
      double[] xFinalControlVector,
      double[] yInitialControlVector,
      double[] yFinalControlVector) {
    super(3);
    this.xInitialControlVector = xInitialControlVector;
    this.xFinalControlVector = xFinalControlVector;
    this.yInitialControlVector = yInitialControlVector;
    this.yFinalControlVector = yFinalControlVector;

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
      // Here, we are multiplying by (3 - i) to manually take the derivative. The
      // power of the term in index 0 is 3, index 1 is 2 and so on. To find the
      // coefficient of the derivative, we can use the power rule and multiply
      // the existing coefficient by its power.
      m_coefficients.set(2, i, m_coefficients.get(0, i) * (3 - i));
      m_coefficients.set(3, i, m_coefficients.get(1, i) * (3 - i));
    }

    for (int i = 0; i < 3; i++) {
      // Here, we are multiplying by (2 - i) to manually take the derivative. The
      // power of the term in index 0 is 2, index 1 is 1 and so on. To find the
      // coefficient of the derivative, we can use the power rule and multiply
      // the existing coefficient by its power.
      m_coefficients.set(4, i, m_coefficients.get(2, i) * (2 - i));
      m_coefficients.set(5, i, m_coefficients.get(3, i) * (2 - i));
    }

    // Assign member variables.
    m_initialControlVector = new ControlVector(xInitialControlVector, yInitialControlVector);
    m_finalControlVector = new ControlVector(xFinalControlVector, yFinalControlVector);
  }

  /**
   * Returns the coefficients matrix.
   *
   * @return The coefficients matrix.
   */
  @Override
  public SimpleMatrix getCoefficients() {
    return m_coefficients;
  }

  /**
   * Returns the initial control vector that created this spline.
   *
   * @return The initial control vector that created this spline.
   */
  @Override
  public ControlVector getInitialControlVector() {
    return m_initialControlVector;
  }

  /**
   * Returns the final control vector that created this spline.
   *
   * @return The final control vector that created this spline.
   */
  @Override
  public ControlVector getFinalControlVector() {
    return m_finalControlVector;
  }

  /**
   * Returns the hermite basis matrix for cubic hermite spline interpolation.
   *
   * @return The hermite basis matrix for cubic hermite spline interpolation.
   */
  @SuppressWarnings("PMD.UnnecessaryVarargsArrayCreation")
  private SimpleMatrix makeHermiteBasis() {
    if (hermiteBasis == null) {
      // Given P(i), P'(i), P(i+1), P'(i+1), the control vectors, we want to find
      // the coefficients of the spline P(t) = a₃t³ + a₂t² + a₁t + a₀.
      //
      // P(i)    = P(0)  = a₀
      // P'(i)   = P'(0) = a₁
      // P(i+1)  = P(1)  = a₃ + a₂ + a₁ + a₀
      // P'(i+1) = P'(1) = 3a₃ + 2a₂ + a₁
      //
      // [P(i)   ] = [0 0 0 1][a₃]
      // [P'(i)  ] = [0 0 1 0][a₂]
      // [P(i+1) ] = [1 1 1 1][a₁]
      // [P'(i+1)] = [3 2 1 0][a₀]
      //
      // To solve for the coefficients, we can invert the 4x4 matrix and move it
      // to the other side of the equation.
      //
      // [a₃] = [ 2  1 -2  1][P(i)   ]
      // [a₂] = [-3 -2  3 -1][P'(i)  ]
      // [a₁] = [ 0  1  0  0][P(i+1) ]
      // [a₀] = [ 1  0  0  0][P'(i+1)]
      hermiteBasis =
          new SimpleMatrix(
              4,
              4,
              true,
              new double[] {
                +2.0, +1.0, -2.0, +1.0, -3.0, -2.0, +3.0, -1.0, +0.0, +1.0, +0.0, +0.0, +1.0, +0.0,
                +0.0, +0.0
              });
    }
    return hermiteBasis;
  }

  /**
   * Returns the control vector for each dimension as a matrix from the user-provided arrays in the
   * constructor.
   *
   * @param initialVector The control vector for the initial point.
   * @param finalVector The control vector for the final point.
   * @return The control vector matrix for a dimension.
   */
  @SuppressWarnings("PMD.UnnecessaryVarargsArrayCreation")
  private SimpleMatrix getControlVectorFromArrays(double[] initialVector, double[] finalVector) {
    if (initialVector.length < 2 || finalVector.length < 2) {
      throw new IllegalArgumentException("Size of vectors must be 2 or greater.");
    }
    return new SimpleMatrix(
        4,
        1,
        true,
        new double[] {
          initialVector[0], initialVector[1],
          finalVector[0], finalVector[1]
        });
  }

  /** CubicHermiteSpline struct for serialization. */
  public static final CubicHermiteSplineProto proto = new CubicHermiteSplineProto();

  /** CubicHermiteSpline protobuf for serialization. */
  public static final CubicHermiteSplineStruct struct = new CubicHermiteSplineStruct();
}
