// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import edu.wpi.first.math.jni.DAREJNI;
import org.ejml.simple.SimpleMatrix;

/** DARE solver utility functions. */
public final class DARE {
  private DARE() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Computes the unique stabilizing solution X to the discrete-time algebraic Riccati equation.
   *
   * <p>AᵀXA − X − AᵀXB(BᵀXB + R)⁻¹BᵀXA + Q = 0
   *
   * <p>This internal function skips expensive precondition checks for increased performance. The
   * solver may hang if any of the following occur:
   *
   * <ul>
   *   <li>Q isn't symmetric positive semidefinite
   *   <li>R isn't symmetric positive definite
   *   <li>The (A, B) pair isn't stabilizable
   *   <li>The (A, C) pair where Q = CᵀC isn't detectable
   * </ul>
   *
   * <p>Only use this function if you're sure the preconditions are met.
   *
   * @param <States> Number of states.
   * @param <Inputs> Number of inputs.
   * @param A System matrix.
   * @param B Input matrix.
   * @param Q State cost matrix.
   * @param R Input cost matrix.
   * @return Solution of DARE.
   */
  public static <States extends Num, Inputs extends Num> Matrix<States, States> dareNoPrecond(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<States, States> Q,
      Matrix<Inputs, Inputs> R) {
    var S = new Matrix<States, States>(new SimpleMatrix(A.getNumRows(), A.getNumCols()));
    DAREJNI.dareNoPrecondABQR(
        A.getStorage().getDDRM().getData(),
        B.getStorage().getDDRM().getData(),
        Q.getStorage().getDDRM().getData(),
        R.getStorage().getDDRM().getData(),
        A.getNumCols(),
        B.getNumCols(),
        S.getStorage().getDDRM().getData());
    return S;
  }

  /**
   * Computes the unique stabilizing solution X to the discrete-time algebraic Riccati equation.
   *
   * <p>AᵀXA − X − (AᵀXB + N)(BᵀXB + R)⁻¹(BᵀXA + Nᵀ) + Q = 0
   *
   * <p>This is equivalent to solving the original DARE:
   *
   * <p>A₂ᵀXA₂ − X − A₂ᵀXB(BᵀXB + R)⁻¹BᵀXA₂ + Q₂ = 0
   *
   * <p>where A₂ and Q₂ are a change of variables:
   *
   * <p>A₂ = A − BR⁻¹Nᵀ and Q₂ = Q − NR⁻¹Nᵀ
   *
   * <p>This overload of the DARE is useful for finding the control law uₖ that minimizes the
   * following cost function subject to xₖ₊₁ = Axₖ + Buₖ.
   *
   * <pre>
   *     ∞ [xₖ]ᵀ[Q  N][xₖ]
   * J = Σ [uₖ] [Nᵀ R][uₖ] ΔT
   *    k=0
   * </pre>
   *
   * <p>This is a more general form of the following. The linear-quadratic regulator is the feedback
   * control law uₖ that minimizes the following cost function subject to xₖ₊₁ = Axₖ + Buₖ:
   *
   * <pre>
   *     ∞
   * J = Σ (xₖᵀQxₖ + uₖᵀRuₖ) ΔT
   *    k=0
   * </pre>
   *
   * <p>This can be refactored as:
   *
   * <pre>
   *     ∞ [xₖ]ᵀ[Q 0][xₖ]
   * J = Σ [uₖ] [0 R][uₖ] ΔT
   *    k=0
   * </pre>
   *
   * <p>This internal function skips expensive precondition checks for increased performance. The
   * solver may hang if any of the following occur:
   *
   * <ul>
   *   <li>Q₂ isn't symmetric positive semidefinite
   *   <li>R isn't symmetric positive definite
   *   <li>The (A₂, B) pair isn't stabilizable
   *   <li>The (A₂, C) pair where Q₂ = CᵀC isn't detectable
   * </ul>
   *
   * <p>Only use this function if you're sure the preconditions are met.
   *
   * @param <States> Number of states.
   * @param <Inputs> Number of inputs.
   * @param A System matrix.
   * @param B Input matrix.
   * @param Q State cost matrix.
   * @param R Input cost matrix.
   * @param N State-input cross-term cost matrix.
   * @return Solution of DARE.
   */
  public static <States extends Num, Inputs extends Num> Matrix<States, States> dareNoPrecond(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<States, States> Q,
      Matrix<Inputs, Inputs> R,
      Matrix<States, Inputs> N) {
    var S = new Matrix<States, States>(new SimpleMatrix(A.getNumRows(), A.getNumCols()));
    DAREJNI.dareNoPrecondABQRN(
        A.getStorage().getDDRM().getData(),
        B.getStorage().getDDRM().getData(),
        Q.getStorage().getDDRM().getData(),
        R.getStorage().getDDRM().getData(),
        N.getStorage().getDDRM().getData(),
        A.getNumCols(),
        B.getNumCols(),
        S.getStorage().getDDRM().getData());
    return S;
  }

  /**
   * Computes the unique stabilizing solution X to the discrete-time algebraic Riccati equation.
   *
   * <p>AᵀXA − X − AᵀXB(BᵀXB + R)⁻¹BᵀXA + Q = 0
   *
   * @param <States> Number of states.
   * @param <Inputs> Number of inputs.
   * @param A System matrix.
   * @param B Input matrix.
   * @param Q State cost matrix.
   * @param R Input cost matrix.
   * @return Solution of DARE.
   * @throws IllegalArgumentException if Q isn't symmetric positive semidefinite.
   * @throws IllegalArgumentException if R isn't symmetric positive definite.
   * @throws IllegalArgumentException if the (A, B) pair isn't stabilizable.
   * @throws IllegalArgumentException if the (A, C) pair where Q = CᵀC isn't detectable.
   */
  public static <States extends Num, Inputs extends Num> Matrix<States, States> dare(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<States, States> Q,
      Matrix<Inputs, Inputs> R) {
    var S = new Matrix<States, States>(new SimpleMatrix(A.getNumRows(), A.getNumCols()));
    DAREJNI.dareABQR(
        A.getStorage().getDDRM().getData(),
        B.getStorage().getDDRM().getData(),
        Q.getStorage().getDDRM().getData(),
        R.getStorage().getDDRM().getData(),
        A.getNumCols(),
        B.getNumCols(),
        S.getStorage().getDDRM().getData());
    return S;
  }

  /**
   * Computes the unique stabilizing solution X to the discrete-time algebraic Riccati equation.
   *
   * <p>AᵀXA − X − (AᵀXB + N)(BᵀXB + R)⁻¹(BᵀXA + Nᵀ) + Q = 0
   *
   * <p>This is equivalent to solving the original DARE:
   *
   * <p>A₂ᵀXA₂ − X − A₂ᵀXB(BᵀXB + R)⁻¹BᵀXA₂ + Q₂ = 0
   *
   * <p>where A₂ and Q₂ are a change of variables:
   *
   * <p>A₂ = A − BR⁻¹Nᵀ and Q₂ = Q − NR⁻¹Nᵀ
   *
   * <p>This overload of the DARE is useful for finding the control law uₖ that minimizes the
   * following cost function subject to xₖ₊₁ = Axₖ + Buₖ.
   *
   * <pre>
   *     ∞ [xₖ]ᵀ[Q  N][xₖ]
   * J = Σ [uₖ] [Nᵀ R][uₖ] ΔT
   *    k=0
   * </pre>
   *
   * <p>This is a more general form of the following. The linear-quadratic regulator is the feedback
   * control law uₖ that minimizes the following cost function subject to xₖ₊₁ = Axₖ + Buₖ:
   *
   * <pre>
   *     ∞
   * J = Σ (xₖᵀQxₖ + uₖᵀRuₖ) ΔT
   *    k=0
   * </pre>
   *
   * <p>This can be refactored as:
   *
   * <pre>
   *     ∞ [xₖ]ᵀ[Q 0][xₖ]
   * J = Σ [uₖ] [0 R][uₖ] ΔT
   *    k=0
   * </pre>
   *
   * @param <States> Number of states.
   * @param <Inputs> Number of inputs.
   * @param A System matrix.
   * @param B Input matrix.
   * @param Q State cost matrix.
   * @param R Input cost matrix.
   * @param N State-input cross-term cost matrix.
   * @return Solution of DARE.
   * @throws IllegalArgumentException if Q₂ isn't symmetric positive semidefinite.
   * @throws IllegalArgumentException if R isn't symmetric positive definite.
   * @throws IllegalArgumentException if the (A₂, B) pair isn't stabilizable.
   * @throws IllegalArgumentException if the (A₂, C) pair where Q₂ = CᵀC isn't detectable.
   */
  public static <States extends Num, Inputs extends Num> Matrix<States, States> dare(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<States, States> Q,
      Matrix<Inputs, Inputs> R,
      Matrix<States, Inputs> N) {
    var S = new Matrix<States, States>(new SimpleMatrix(A.getNumRows(), A.getNumCols()));
    DAREJNI.dareABQRN(
        A.getStorage().getDDRM().getData(),
        B.getStorage().getDDRM().getData(),
        Q.getStorage().getDDRM().getData(),
        R.getStorage().getDDRM().getData(),
        N.getStorage().getDDRM().getData(),
        A.getNumCols(),
        B.getNumCols(),
        S.getStorage().getDDRM().getData());
    return S;
  }
}
