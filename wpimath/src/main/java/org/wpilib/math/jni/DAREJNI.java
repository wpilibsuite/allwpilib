// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.jni;

/** DARE JNI. */
public final class DAREJNI extends WPIMathJNI {
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
   * <p>Only use this function if you're sure the preconditions are met. Solves the discrete
   * algebraic Riccati equation.
   *
   * @param A Array containing elements of A in row-major order.
   * @param B Array containing elements of B in row-major order.
   * @param Q Array containing elements of Q in row-major order.
   * @param R Array containing elements of R in row-major order.
   * @param states Number of states in A matrix.
   * @param inputs Number of inputs in B matrix.
   * @param S Array storage for DARE solution.
   */
  public static native void dareNoPrecondABQR(
      double[] A, double[] B, double[] Q, double[] R, int states, int inputs, double[] S);

  /**
   * Computes the unique stabilizing solution X to the discrete-time algebraic Riccati equation.
   *
   * <p>AᵀXA − X − (AᵀXB + N)(BᵀXB + R)⁻¹(BᵀXA + Nᵀ) + Q = 0
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
   *   <li>Q − NR⁻¹Nᵀ isn't symmetric positive semidefinite
   *   <li>R isn't symmetric positive definite
   *   <li>The (A − BR⁻¹Nᵀ, B) pair isn't stabilizable
   *   <li>The (A, C) pair where Q = CᵀC isn't detectable
   * </ul>
   *
   * <p>Only use this function if you're sure the preconditions are met.
   *
   * @param A Array containing elements of A in row-major order.
   * @param B Array containing elements of B in row-major order.
   * @param Q Array containing elements of Q in row-major order.
   * @param R Array containing elements of R in row-major order.
   * @param N Array containing elements of N in row-major order.
   * @param states Number of states in A matrix.
   * @param inputs Number of inputs in B matrix.
   * @param S Array storage for DARE solution.
   */
  public static native void dareNoPrecondABQRN(
      double[] A,
      double[] B,
      double[] Q,
      double[] R,
      double[] N,
      int states,
      int inputs,
      double[] S);

  /**
   * Computes the unique stabilizing solution X to the discrete-time algebraic Riccati equation.
   *
   * <p>AᵀXA − X − AᵀXB(BᵀXB + R)⁻¹BᵀXA + Q = 0
   *
   * @param A Array containing elements of A in row-major order.
   * @param B Array containing elements of B in row-major order.
   * @param Q Array containing elements of Q in row-major order.
   * @param R Array containing elements of R in row-major order.
   * @param states Number of states in A matrix.
   * @param inputs Number of inputs in B matrix.
   * @param S Array storage for DARE solution.
   * @throws IllegalArgumentException if Q isn't symmetric positive semidefinite.
   * @throws IllegalArgumentException if R isn't symmetric positive definite.
   * @throws IllegalArgumentException if the (A, B) pair isn't stabilizable.
   * @throws IllegalArgumentException if the (A, C) pair where Q = CᵀC isn't detectable.
   */
  public static native void dareABQR(
      double[] A, double[] B, double[] Q, double[] R, int states, int inputs, double[] S);

  /**
   * Computes the unique stabilizing solution X to the discrete-time algebraic Riccati equation.
   *
   * <p>AᵀXA − X − (AᵀXB + N)(BᵀXB + R)⁻¹(BᵀXA + Nᵀ) + Q = 0
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
   * @param A Array containing elements of A in row-major order.
   * @param B Array containing elements of B in row-major order.
   * @param Q Array containing elements of Q in row-major order.
   * @param R Array containing elements of R in row-major order.
   * @param N Array containing elements of N in row-major order.
   * @param states Number of states in A matrix.
   * @param inputs Number of inputs in B matrix.
   * @param S Array storage for DARE solution.
   * @throws IllegalArgumentException if Q − NR⁻¹Nᵀ isn't symmetric positive semidefinite.
   * @throws IllegalArgumentException if R isn't symmetric positive definite.
   * @throws IllegalArgumentException if the (A − BR⁻¹Nᵀ, B) pair isn't stabilizable.
   * @throws IllegalArgumentException if the (A, C) pair where Q = CᵀC isn't detectable.
   */
  public static native void dareABQRN(
      double[] A,
      double[] B,
      double[] Q,
      double[] R,
      double[] N,
      int states,
      int inputs,
      double[] S);

  /** Utility class. */
  private DAREJNI() {}
}
