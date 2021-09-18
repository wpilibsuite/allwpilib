// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.Pair;
import org.ejml.simple.SimpleMatrix;

@SuppressWarnings({"ParameterName", "MethodTypeParameterName"})
public final class Discretization {
  private Discretization() {
    // Utility class
  }

  /**
   * Discretizes the given continuous A matrix.
   *
   * @param <States> Num representing the number of states.
   * @param contA Continuous system matrix.
   * @param dtSeconds Discretization timestep.
   * @return the discrete matrix system.
   */
  public static <States extends Num> Matrix<States, States> discretizeA(
      Matrix<States, States> contA, double dtSeconds) {
    return contA.times(dtSeconds).exp();
  }

  /**
   * Discretizes the given continuous A and B matrices.
   *
   * @param <States> Nat representing the states of the system.
   * @param <Inputs> Nat representing the inputs to the system.
   * @param contA Continuous system matrix.
   * @param contB Continuous input matrix.
   * @param dtSeconds Discretization timestep.
   * @return a Pair representing discA and diskB.
   */
  @SuppressWarnings("LocalVariableName")
  public static <States extends Num, Inputs extends Num>
      Pair<Matrix<States, States>, Matrix<States, Inputs>> discretizeAB(
          Matrix<States, States> contA, Matrix<States, Inputs> contB, double dtSeconds) {
    var scaledA = contA.times(dtSeconds);
    var scaledB = contB.times(dtSeconds);

    int states = contA.getNumRows();
    int inputs = contB.getNumCols();
    var M = new Matrix<>(new SimpleMatrix(states + inputs, states + inputs));
    M.assignBlock(0, 0, scaledA);
    M.assignBlock(0, scaledA.getNumCols(), scaledB);
    var phi = M.exp();

    var discA = new Matrix<States, States>(new SimpleMatrix(states, states));
    var discB = new Matrix<States, Inputs>(new SimpleMatrix(states, inputs));

    discA.extractFrom(0, 0, phi);
    discB.extractFrom(0, contB.getNumRows(), phi);

    return new Pair<>(discA, discB);
  }

  /**
   * Discretizes the given continuous A and Q matrices.
   *
   * @param <States> Nat representing the number of states.
   * @param contA Continuous system matrix.
   * @param contQ Continuous process noise covariance matrix.
   * @param dtSeconds Discretization timestep.
   * @return a pair representing the discrete system matrix and process noise covariance matrix.
   */
  @SuppressWarnings("LocalVariableName")
  public static <States extends Num>
      Pair<Matrix<States, States>, Matrix<States, States>> discretizeAQ(
          Matrix<States, States> contA, Matrix<States, States> contQ, double dtSeconds) {
    int states = contA.getNumRows();

    // Make continuous Q symmetric if it isn't already
    Matrix<States, States> Q = contQ.plus(contQ.transpose()).div(2.0);

    // Set up the matrix M = [[-A, Q], [0, A.T]]
    final var M = new Matrix<>(new SimpleMatrix(2 * states, 2 * states));
    M.assignBlock(0, 0, contA.times(-1.0));
    M.assignBlock(0, states, Q);
    M.assignBlock(states, 0, new Matrix<>(new SimpleMatrix(states, states)));
    M.assignBlock(states, states, contA.transpose());

    final var phi = M.times(dtSeconds).exp();

    // Phi12 = phi[0:States,        States:2*States]
    // Phi22 = phi[States:2*States, States:2*States]
    final Matrix<States, States> phi12 = phi.block(states, states, 0, states);
    final Matrix<States, States> phi22 = phi.block(states, states, states, states);

    final var discA = phi22.transpose();

    Q = discA.times(phi12);

    // Make discrete Q symmetric if it isn't already
    final var discQ = Q.plus(Q.transpose()).div(2.0);

    return new Pair<>(discA, discQ);
  }

  /**
   * Discretizes the given continuous A and Q matrices.
   *
   * <p>Rather than solving a 2N x 2N matrix exponential like in DiscretizeQ() (which is expensive),
   * we take advantage of the structure of the block matrix of A and Q.
   *
   * <p>The exponential of A*t, which is only N x N, is relatively cheap. 2) The upper-right quarter
   * of the 2N x 2N matrix, which we can approximate using a taylor series to several terms and
   * still be substantially cheaper than taking the big exponential.
   *
   * @param <States> Nat representing the number of states.
   * @param contA Continuous system matrix.
   * @param contQ Continuous process noise covariance matrix.
   * @param dtSeconds Discretization timestep.
   * @return a pair representing the discrete system matrix and process noise covariance matrix.
   */
  @SuppressWarnings("LocalVariableName")
  public static <States extends Num>
      Pair<Matrix<States, States>, Matrix<States, States>> discretizeAQTaylor(
          Matrix<States, States> contA, Matrix<States, States> contQ, double dtSeconds) {
    // Make continuous Q symmetric if it isn't already
    Matrix<States, States> Q = contQ.plus(contQ.transpose()).div(2.0);

    Matrix<States, States> lastTerm = Q.copy();
    double lastCoeff = dtSeconds;

    // Aᵀⁿ
    Matrix<States, States> Atn = contA.transpose();
    Matrix<States, States> phi12 = lastTerm.times(lastCoeff);

    // i = 6 i.e. 5th order should be enough precision
    for (int i = 2; i < 6; ++i) {
      lastTerm = contA.times(-1).times(lastTerm).plus(Q.times(Atn));
      lastCoeff *= dtSeconds / ((double) i);

      phi12 = phi12.plus(lastTerm.times(lastCoeff));

      Atn = Atn.times(contA.transpose());
    }

    var discA = discretizeA(contA, dtSeconds);
    Q = discA.times(phi12);

    // Make Q symmetric if it isn't already
    var discQ = Q.plus(Q.transpose()).div(2.0);

    return new Pair<>(discA, discQ);
  }

  /**
   * Returns a discretized version of the provided continuous measurement noise covariance matrix.
   * Note that dt=0.0 divides R by zero.
   *
   * @param <O> Nat representing the number of outputs.
   * @param R Continuous measurement noise covariance matrix.
   * @param dtSeconds Discretization timestep.
   * @return Discretized version of the provided continuous measurement noise covariance matrix.
   */
  public static <O extends Num> Matrix<O, O> discretizeR(Matrix<O, O> R, double dtSeconds) {
    return R.div(dtSeconds);
  }
}
