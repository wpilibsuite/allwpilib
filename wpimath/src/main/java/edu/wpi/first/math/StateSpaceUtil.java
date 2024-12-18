// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.jni.StateSpaceUtilJNI;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.math.numbers.N4;
import java.util.Random;
import org.ejml.simple.SimpleMatrix;

/** State-space utilities. */
public final class StateSpaceUtil {
  private static Random rand = new Random();

  private StateSpaceUtil() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Creates a covariance matrix from the given vector for use with Kalman filters.
   *
   * <p>Each element is squared and placed on the covariance matrix diagonal.
   *
   * @param <States> Num representing the states of the system.
   * @param states A Nat representing the states of the system.
   * @param stdDevs For a Q matrix, its elements are the standard deviations of each state from how
   *     the model behaves. For an R matrix, its elements are the standard deviations for each
   *     output measurement.
   * @return Process noise or measurement noise covariance matrix.
   */
  public static <States extends Num> Matrix<States, States> makeCovarianceMatrix(
      Nat<States> states, Matrix<States, N1> stdDevs) {
    var result = new Matrix<>(states, states);
    for (int i = 0; i < states.getNum(); i++) {
      result.set(i, i, Math.pow(stdDevs.get(i, 0), 2));
    }
    return result;
  }

  /**
   * Creates a vector of normally distributed white noise with the given noise intensities for each
   * element.
   *
   * @param <N> Num representing the dimensionality of the noise vector to create.
   * @param stdDevs A matrix whose elements are the standard deviations of each element of the noise
   *     vector.
   * @return White noise vector.
   */
  public static <N extends Num> Matrix<N, N1> makeWhiteNoiseVector(Matrix<N, N1> stdDevs) {
    Matrix<N, N1> result = new Matrix<>(new SimpleMatrix(stdDevs.getNumRows(), 1));
    for (int i = 0; i < stdDevs.getNumRows(); i++) {
      result.set(i, 0, rand.nextGaussian() * stdDevs.get(i, 0));
    }
    return result;
  }

  /**
   * Creates a cost matrix from the given vector for use with LQR.
   *
   * <p>The cost matrix is constructed using Bryson's rule. The inverse square of each tolerance is
   * placed on the cost matrix diagonal. If a tolerance is infinity, its cost matrix entry is set to
   * zero.
   *
   * @param <Elements> Nat representing the number of system states or inputs.
   * @param tolerances An array. For a Q matrix, its elements are the maximum allowed excursions of
   *     the states from the reference. For an R matrix, its elements are the maximum allowed
   *     excursions of the control inputs from no actuation.
   * @return State excursion or control effort cost matrix.
   */
  public static <Elements extends Num> Matrix<Elements, Elements> makeCostMatrix(
      Matrix<Elements, N1> tolerances) {
    Matrix<Elements, Elements> result =
        new Matrix<>(new SimpleMatrix(tolerances.getNumRows(), tolerances.getNumRows()));
    result.fill(0.0);

    for (int i = 0; i < tolerances.getNumRows(); i++) {
      if (tolerances.get(i, 0) == Double.POSITIVE_INFINITY) {
        result.set(i, i, 0.0);
      } else {
        result.set(i, i, 1.0 / Math.pow(tolerances.get(i, 0), 2));
      }
    }

    return result;
  }

  /**
   * Returns true if (A, B) is a stabilizable pair.
   *
   * <p>(A, B) is stabilizable if and only if the uncontrollable eigenvalues of A, if any, have
   * absolute values less than one, where an eigenvalue is uncontrollable if rank([λI - A, B]) %3C n
   * where n is the number of states.
   *
   * @param <States> Num representing the size of A.
   * @param <Inputs> Num representing the columns of B.
   * @param A System matrix.
   * @param B Input matrix.
   * @return If the system is stabilizable.
   */
  public static <States extends Num, Inputs extends Num> boolean isStabilizable(
      Matrix<States, States> A, Matrix<States, Inputs> B) {
    return StateSpaceUtilJNI.isStabilizable(
        A.getNumRows(), B.getNumCols(), A.getData(), B.getData());
  }

  /**
   * Returns true if (A, C) is a detectable pair.
   *
   * <p>(A, C) is detectable if and only if the unobservable eigenvalues of A, if any, have absolute
   * values less than one, where an eigenvalue is unobservable if rank([λI - A; C]) %3C n where n is
   * the number of states.
   *
   * @param <States> Num representing the size of A.
   * @param <Outputs> Num representing the rows of C.
   * @param A System matrix.
   * @param C Output matrix.
   * @return If the system is detectable.
   */
  public static <States extends Num, Outputs extends Num> boolean isDetectable(
      Matrix<States, States> A, Matrix<Outputs, States> C) {
    return StateSpaceUtilJNI.isStabilizable(
        A.getNumRows(), C.getNumRows(), A.transpose().getData(), C.transpose().getData());
  }

  /**
   * Convert a {@link Pose2d} to a vector of [x, y, theta], where theta is in radians.
   *
   * @param pose A pose to convert to a vector.
   * @return The given pose in vector form, with the third element, theta, in radians.
   * @deprecated Create the vector manually instead. If you were using this as an intermediate step
   *     for constructing affine transformations, use {@link Pose2d#toMatrix()} instead.
   */
  @Deprecated(forRemoval = true, since = "2025")
  public static Matrix<N3, N1> poseToVector(Pose2d pose) {
    return VecBuilder.fill(pose.getX(), pose.getY(), pose.getRotation().getRadians());
  }

  /**
   * Clamp the input u to the min and max.
   *
   * @param u The input to clamp.
   * @param umin The minimum input magnitude.
   * @param umax The maximum input magnitude.
   * @param <I> Number of inputs.
   * @return The clamped input.
   */
  public static <I extends Num> Matrix<I, N1> clampInputMaxMagnitude(
      Matrix<I, N1> u, Matrix<I, N1> umin, Matrix<I, N1> umax) {
    var result = new Matrix<I, N1>(new SimpleMatrix(u.getNumRows(), 1));
    for (int i = 0; i < u.getNumRows(); i++) {
      result.set(i, 0, MathUtil.clamp(u.get(i, 0), umin.get(i, 0), umax.get(i, 0)));
    }
    return result;
  }

  /**
   * Renormalize all inputs if any exceeds the maximum magnitude. Useful for systems such as
   * differential drivetrains.
   *
   * @param u The input vector.
   * @param maxMagnitude The maximum magnitude any input can have.
   * @param <I> Number of inputs.
   * @return The normalizedInput
   */
  public static <I extends Num> Matrix<I, N1> desaturateInputVector(
      Matrix<I, N1> u, double maxMagnitude) {
    double maxValue = u.maxAbs();
    boolean isCapped = maxValue > maxMagnitude;

    if (isCapped) {
      return u.times(maxMagnitude / maxValue);
    }
    return u;
  }

  /**
   * Convert a {@link Pose2d} to a vector of [x, y, cos(theta), sin(theta)], where theta is in
   * radians.
   *
   * @param pose A pose to convert to a vector.
   * @return The given pose in as a 4x1 vector of x, y, cos(theta), and sin(theta).
   * @deprecated Create the vector manually instead. If you were using this as an intermediate step
   *     for constructing affine transformations, use {@link Pose2d#toMatrix()} instead.
   */
  @Deprecated(forRemoval = true, since = "2025")
  public static Matrix<N4, N1> poseTo4dVector(Pose2d pose) {
    return VecBuilder.fill(
        pose.getTranslation().getX(),
        pose.getTranslation().getY(),
        pose.getRotation().getCos(),
        pose.getRotation().getSin());
  }

  /**
   * Convert a {@link Pose2d} to a vector of [x, y, theta], where theta is in radians.
   *
   * @param pose A pose to convert to a vector.
   * @return The given pose in vector form, with the third element, theta, in radians.
   * @deprecated Create the vector manually instead. If you were using this as an intermediate step
   *     for constructing affine transformations, use {@link Pose2d#toMatrix()} instead.
   */
  @Deprecated(forRemoval = true, since = "2025")
  public static Matrix<N3, N1> poseTo3dVector(Pose2d pose) {
    return VecBuilder.fill(
        pose.getTranslation().getX(),
        pose.getTranslation().getY(),
        pose.getRotation().getRadians());
  }
}
