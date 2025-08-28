// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.system;

import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.util.Num;
import org.wpilib.math.util.Pair;
import org.ejml.simple.SimpleMatrix;

/** Discretization helper functions. */
public final class Discretization {
  private Discretization() {
    // Utility class
  }

  /**
   * Discretizes the given continuous A matrix.
   *
   * @param <States> Num representing the number of states.
   * @param contA Continuous system matrix.
   * @param dt Discretization timestep in seconds.
   * @return the discrete matrix system.
   */
  public static <States extends Num> Matrix<States, States> discretizeA(
      Matrix<States, States> contA, double dt) {
    // A_d = eᴬᵀ
    return contA.times(dt).exp();
  }

  /**
   * Discretizes the given continuous A and B matrices.
   *
   * @param <States> Nat representing the states of the system.
   * @param <Inputs> Nat representing the inputs to the system.
   * @param contA Continuous system matrix.
   * @param contB Continuous input matrix.
   * @param dt Discretization timestep in seconds.
   * @return a Pair representing discA and diskB.
   */
  public static <States extends Num, Inputs extends Num>
      Pair<Matrix<States, States>, Matrix<States, Inputs>> discretizeAB(
          Matrix<States, States> contA, Matrix<States, Inputs> contB, double dt) {
    int states = contA.getNumRows();
    int inputs = contB.getNumCols();

    // M = [A  B]
    //     [0  0]
    var M = new Matrix<>(new SimpleMatrix(states + inputs, states + inputs));
    M.assignBlock(0, 0, contA);
    M.assignBlock(0, contA.getNumCols(), contB);

    //  ϕ = eᴹᵀ = [A_d  B_d]
    //            [ 0    I ]
    var phi = M.times(dt).exp();

    var discA = new Matrix<States, States>(new SimpleMatrix(states, states));
    discA.extractFrom(0, 0, phi);

    var discB = new Matrix<States, Inputs>(new SimpleMatrix(states, inputs));
    discB.extractFrom(0, contB.getNumRows(), phi);

    return new Pair<>(discA, discB);
  }

  /**
   * Discretizes the given continuous A and Q matrices.
   *
   * @param <States> Nat representing the number of states.
   * @param contA Continuous system matrix.
   * @param contQ Continuous process noise covariance matrix.
   * @param dt Discretization timestep in seconds.
   * @return a pair representing the discrete system matrix and process noise covariance matrix.
   */
  public static <States extends Num>
      Pair<Matrix<States, States>, Matrix<States, States>> discretizeAQ(
          Matrix<States, States> contA, Matrix<States, States> contQ, double dt) {
    int states = contA.getNumRows();

    // Make continuous Q symmetric if it isn't already
    Matrix<States, States> Q = contQ.plus(contQ.transpose()).div(2.0);

    // M = [−A  Q ]
    //     [ 0  Aᵀ]
    final var M = new Matrix<>(new SimpleMatrix(2 * states, 2 * states));
    M.assignBlock(0, 0, contA.times(-1.0));
    M.assignBlock(0, states, Q);
    M.assignBlock(states, 0, new Matrix<>(new SimpleMatrix(states, states)));
    M.assignBlock(states, states, contA.transpose());

    // ϕ = eᴹᵀ = [−A_d  A_d⁻¹Q_d]
    //           [ 0      A_dᵀ  ]
    final var phi = M.times(dt).exp();

    // ϕ₁₂ = A_d⁻¹Q_d
    final Matrix<States, States> phi12 = phi.block(states, states, 0, states);

    // ϕ₂₂ = A_dᵀ
    final Matrix<States, States> phi22 = phi.block(states, states, states, states);

    final var discA = phi22.transpose();

    Q = discA.times(phi12);

    // Make discrete Q symmetric if it isn't already
    final var discQ = Q.plus(Q.transpose()).div(2.0);

    return new Pair<>(discA, discQ);
  }

  /**
   * Returns a discretized version of the provided continuous measurement noise covariance matrix.
   * Note that dt=0.0 divides R by zero.
   *
   * @param <O> Nat representing the number of outputs.
   * @param contR Continuous measurement noise covariance matrix.
   * @param dt Discretization timestep in seconds.
   * @return Discretized version of the provided continuous measurement noise covariance matrix.
   */
  public static <O extends Num> Matrix<O, O> discretizeR(Matrix<O, O> contR, double dt) {
    // R_d = 1/T R
    return contR.div(dt);
  }
}
