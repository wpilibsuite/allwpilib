/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.math;

import java.util.Random;

import org.ejml.simple.SimpleMatrix;

import edu.wpi.first.math.WPIMathJNI;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpiutil.math.MathUtil;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N3;
import edu.wpi.first.wpiutil.math.numbers.N4;

@SuppressWarnings("ParameterName")
public final class StateSpaceUtil {
  private StateSpaceUtil() {
    // Utility class
  }

  /**
   * Creates a covariance matrix from the given vector for use with Kalman
   * filters.
   *
   * <p>Each element is squared and placed on the covariance matrix diagonal.
   *
   * @param <States>     Num representing the states of the system.
   * @param states  A Nat representing the states of the system.
   * @param stdDevs For a Q matrix, its elements are the standard deviations of
   *                each state from how the model behaves. For an R matrix, its
   *                elements are the standard deviations for each output
   *                measurement.
   * @return Process noise or measurement noise covariance matrix.
   */
  @SuppressWarnings("MethodTypeParameterName")
  public static <States extends Num> Matrix<States, States> makeCovarianceMatrix(
      Nat<States> states, Matrix<States, N1> stdDevs
  ) {
    var result = new Matrix<>(states, states);
    for (int i = 0; i < states.getNum(); i++) {
      result.set(i, i, Math.pow(stdDevs.get(i, 0), 2));
    }
    return result;
  }

  /**
   * Creates a vector of normally distributed white noise with the given noise
   * intensities for each element.
   *
   * @param <N>     Num representing the dimensionality of  the noise vector to create.
   * @param stdDevs A matrix whose elements are the standard deviations of each
   *                element of the noise vector.
   * @return White noise vector.
   */
  public static <N extends Num> Matrix<N, N1> makeWhiteNoiseVector(
        Matrix<N, N1> stdDevs
  ) {
    var rand = new Random();

    Matrix<N, N1> result = new Matrix<>(new SimpleMatrix(stdDevs.getNumRows(), 1));
    for (int i = 0; i < stdDevs.getNumRows(); i++) {
      result.set(i, 0, rand.nextGaussian() * stdDevs.get(i, 0));
    }
    return result;
  }

  /**
   * Creates a cost matrix from the given vector for use with LQR.
   *
   * <p>The cost matrix is constructed using Bryson's rule. The inverse square of
   * each element in the input is taken and placed on the cost matrix diagonal.
   *
   * @param <States>   Nat representing the states of the system.
   * @param costs An array. For a Q matrix, its elements are the maximum allowed
   *              excursions of the states from the reference. For an R matrix,
   *              its elements are the maximum allowed excursions of the control
   *              inputs from no actuation.
   * @return State excursion or control effort cost matrix.
   */
  @SuppressWarnings("MethodTypeParameterName")
  public static <States extends Num> Matrix<States, States>
      makeCostMatrix(Matrix<States, N1> costs) {
    Matrix<States, States> result =
        new Matrix<>(new SimpleMatrix(costs.getNumRows(), costs.getNumRows()));
    result.fill(0.0);

    for (int i = 0; i < costs.getNumRows(); i++) {
      result.set(i, i, 1.0 / (Math.pow(costs.get(i, 0), 2)));
    }

    return result;
  }

  /**
   * Returns true if (A, B) is a stabilizable pair.
   *
   * <p>(A,B) is stabilizable if and only if the uncontrollable eigenvalues of A, if
   * any, have absolute values less than one, where an eigenvalue is
   * uncontrollable if rank(lambda * I - A, B) %3C n where n is number of states.
   *
   * @param <States> Num representing the size of A.
   * @param <Inputs> Num representing the columns of B.
   * @param A   System matrix.
   * @param B   Input matrix.
   * @return If the system is stabilizable.
   */
  @SuppressWarnings("MethodTypeParameterName")
  public static <States extends Num, Inputs extends Num> boolean isStabilizable(
      Matrix<States, States> A, Matrix<States, Inputs> B) {
    return WPIMathJNI.isStabilizable(A.getNumRows(), B.getNumCols(),
            A.getData(), B.getData());
  }

  /**
   * Convert a {@link Pose2d} to a vector of [x, y, theta], where theta is in radians.
   *
   * @param pose A pose to convert to a vector.
   * @return The given pose in vector form, with the third element, theta, in radians.
   */
  public static Matrix<N3, N1> poseToVector(Pose2d pose) {
    return VecBuilder.fill(
            pose.getX(),
            pose.getY(),
            pose.getRotation().getRadians()
    );
  }

  /**
   * Clamp the input u to the min and max.
   *
   * @param u    The input to clamp.
   * @param umin The minimum input magnitude.
   * @param umax The maximum input magnitude.
   * @param <I>  The number of inputs.
   * @return     The clamped input.
   */
  @SuppressWarnings({"ParameterName", "LocalVariableName"})
  public static <I extends Num> Matrix<I, N1> clampInputMaxMagnitude(Matrix<I, N1> u,
                                                                     Matrix<I, N1> umin,
                                                                     Matrix<I, N1> umax) {
    var result = new Matrix<I, N1>(new SimpleMatrix(u.getNumRows(), 1));
    for (int i = 0; i < u.getNumRows(); i++) {
      result.set(i, 0, MathUtil.clamp(
            u.get(i, 0),
            umin.get(i, 0),
            umax.get(i, 0)));
    }
    return result;
  }

  /**
   * Normalize all inputs if any excedes the maximum magnitude. Useful for systems such as
   * differential drivetrains.
   *
   * @param u            The input vector.
   * @param maxMagnitude The maximum magnitude any input can have.
   * @param <I>          The number of inputs.
   * @return The normalizedInput
   */
  public static <I extends Num> Matrix<I, N1> normalizeInputVector(Matrix<I, N1> u,
                                                                   double maxMagnitude) {
    double maxValue = u.maxAbs();
    boolean isCapped = maxValue > maxMagnitude;

    if (isCapped) {
      return u.times(maxMagnitude / maxValue);
    }
    return u;
  }

  /**
   * Convert a {@link Pose2d} to a vector of [x, y, cos(theta), sin(theta)],
   * where theta is in radians.
   *
   * @param pose A pose to convert to a vector.
   */
  public static Matrix<N4, N1> poseTo4dVector(Pose2d pose) {
    return VecBuilder.fill(
      pose.getTranslation().getX(),
      pose.getTranslation().getY(),
      pose.getRotation().getCos(),
      pose.getRotation().getSin()
    );
  }

  /**
   * Convert a {@link Pose2d} to a vector of [x, y, theta], where theta is in radians.
   *
   * @param pose A pose to convert to a vector.
   * @return The given pose in vector form, with the third element, theta, in radians.
   */
  public static Matrix<N3, N1> poseTo3dVector(Pose2d pose) {
    return VecBuilder.fill(
      pose.getTranslation().getX(),
      pose.getTranslation().getY(),
      pose.getRotation().getRadians()
    );
  }
}
