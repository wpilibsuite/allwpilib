// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.spline;

import edu.wpi.first.math.spline.proto.QuinticHermiteSplineProto;
import edu.wpi.first.math.spline.struct.QuinticHermiteSplineStruct;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;
import org.ejml.simple.SimpleMatrix;

/** Represents a hermite spline of degree 5. */
public class QuinticHermiteSpline extends Spline
    implements ProtobufSerializable, StructSerializable {
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
   * Constructs a quintic hermite spline with the specified control vectors. Each control vector
   * contains into about the location of the point, its first derivative, and its second derivative.
   *
   * @param xInitialControlVector The control vector for the initial point in the x dimension.
   * @param xFinalControlVector The control vector for the final point in the x dimension.
   * @param yInitialControlVector The control vector for the initial point in the y dimension.
   * @param yFinalControlVector The control vector for the final point in the y dimension.
   */
  @SuppressWarnings("PMD.ArrayIsStoredDirectly")
  public QuinticHermiteSpline(
      double[] xInitialControlVector,
      double[] xFinalControlVector,
      double[] yInitialControlVector,
      double[] yFinalControlVector) {
    super(5);
    this.xInitialControlVector = xInitialControlVector;
    this.yInitialControlVector = yInitialControlVector;
    this.xFinalControlVector = xFinalControlVector;
    this.yFinalControlVector = yFinalControlVector;

    // Populate the coefficients for the actual spline equations.
    // Row 0 is x coefficients
    // Row 1 is y coefficients
    final var hermite = makeHermiteBasis();
    final var x = getControlVectorFromArrays(xInitialControlVector, xFinalControlVector);
    final var y = getControlVectorFromArrays(yInitialControlVector, yFinalControlVector);

    final var xCoeffs = (hermite.mult(x)).transpose();
    final var yCoeffs = (hermite.mult(y)).transpose();

    m_coefficients = new SimpleMatrix(6, 6);

    for (int i = 0; i < 6; i++) {
      m_coefficients.set(0, i, xCoeffs.get(0, i));
      m_coefficients.set(1, i, yCoeffs.get(0, i));
    }
    for (int i = 0; i < 6; i++) {
      // Populate Row 2 and Row 3 with the derivatives of the equations above.
      // Here, we are multiplying by (5 - i) to manually take the derivative. The
      // power of the term in index 0 is 5, index 1 is 4 and so on. To find the
      // coefficient of the derivative, we can use the power rule and multiply
      // the existing coefficient by its power.
      m_coefficients.set(2, i, m_coefficients.get(0, i) * (5 - i));
      m_coefficients.set(3, i, m_coefficients.get(1, i) * (5 - i));
    }
    for (int i = 0; i < 5; i++) {
      // Then populate row 4 and 5 with the second derivatives.
      // Here, we are multiplying by (4 - i) to manually take the derivative. The
      // power of the term in index 0 is 4, index 1 is 3 and so on. To find the
      // coefficient of the derivative, we can use the power rule and multiply
      // the existing coefficient by its power.
      m_coefficients.set(4, i, m_coefficients.get(2, i) * (4 - i));
      m_coefficients.set(5, i, m_coefficients.get(3, i) * (4 - i));
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
   * Returns the hermite basis matrix for quintic hermite spline interpolation.
   *
   * @return The hermite basis matrix for quintic hermite spline interpolation.
   */
  @SuppressWarnings("PMD.UnnecessaryVarargsArrayCreation")
  private SimpleMatrix makeHermiteBasis() {
    if (hermiteBasis == null) {
      // Given P(i), P'(i), P"(i), P(i+1), P'(i+1), P"(i+1), the control vectors,
      // we want to find the coefficients of the spline
      // P(t) = a₅t⁵ + a₄t⁴ + a₃t³ + a₂t² + a₁t + a₀.
      //
      // P(i)    = P(0)  = a₀
      // P'(i)   = P'(0) = a₁
      // P''(i)  = P"(0) = 2a₂
      // P(i+1)  = P(1)  = a₅ + a₄ + a₃ + a₂ + a₁ + a₀
      // P'(i+1) = P'(1) = 5a₅ + 4a₄ + 3a₃ + 2a₂ + a₁
      // P"(i+1) = P"(1) = 20a₅ + 12a₄ + 6a₃ + 2a₂
      //
      // [P(i)   ] = [ 0  0  0  0  0  1][a₅]
      // [P'(i)  ] = [ 0  0  0  0  1  0][a₄]
      // [P"(i)  ] = [ 0  0  0  2  0  0][a₃]
      // [P(i+1) ] = [ 1  1  1  1  1  1][a₂]
      // [P'(i+1)] = [ 5  4  3  2  1  0][a₁]
      // [P"(i+1)] = [20 12  6  2  0  0][a₀]
      //
      // To solve for the coefficients, we can invert the 6x6 matrix and move it
      // to the other side of the equation.
      //
      // [a₅] = [ -6.0  -3.0  -0.5   6.0  -3.0   0.5][P(i)   ]
      // [a₄] = [ 15.0   8.0   1.5 -15.0   7.0  -1.0][P'(i)  ]
      // [a₃] = [-10.0  -6.0  -1.5  10.0  -4.0   0.5][P"(i)  ]
      // [a₂] = [  0.0   0.0   0.5   0.0   0.0   0.0][P(i+1) ]
      // [a₁] = [  0.0   1.0   0.0   0.0   0.0   0.0][P'(i+1)]
      // [a₀] = [  1.0   0.0   0.0   0.0   0.0   0.0][P"(i+1)]
      hermiteBasis =
          new SimpleMatrix(
              6,
              6,
              true,
              new double[] {
                -06.0, -03.0, -00.5, +06.0, -03.0, +00.5, +15.0, +08.0, +01.5, -15.0, +07.0, -01.0,
                -10.0, -06.0, -01.5, +10.0, -04.0, +00.5, +00.0, +00.0, +00.5, +00.0, +00.0, +00.0,
                +00.0, +01.0, +00.0, +00.0, +00.0, +00.0, +01.0, +00.0, +00.0, +00.0, +00.0, +00.0
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
    if (initialVector.length != 3 || finalVector.length != 3) {
      throw new IllegalArgumentException("Size of vectors must be 3");
    }
    return new SimpleMatrix(
        6,
        1,
        true,
        new double[] {
          initialVector[0], initialVector[1], initialVector[2],
          finalVector[0], finalVector[1], finalVector[2]
        });
  }

  /** QuinticHermiteSpline struct for serialization. */
  public static final QuinticHermiteSplineProto proto = new QuinticHermiteSplineProto();

  /** QuinticHermiteSpline protobuf for serialization. */
  public static final QuinticHermiteSplineStruct struct = new QuinticHermiteSplineStruct();
}
