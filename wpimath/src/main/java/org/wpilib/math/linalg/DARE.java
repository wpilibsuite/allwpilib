// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.linalg;

import org.ejml.LinearSolverSafe;
import org.ejml.data.DMatrixRMaj;
import org.ejml.dense.row.factory.LinearSolverFactory_DDRM;
import org.ejml.interfaces.linsol.LinearSolverDense;
import org.ejml.simple.SimpleMatrix;
import org.wpilib.math.system.LinearSystemUtil;
import org.wpilib.math.util.Num;

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
   * @param A System matrix.
   * @param B Input matrix.
   * @param Q State cost matrix.
   * @param R_llt The LLT decomposition of the input cost matrix.
   * @return Solution of DARE.
   */
  private static <States extends Num> Matrix<States, States> dareDetail(
      SimpleMatrix A, SimpleMatrix B, SimpleMatrix Q, LinearSolverDense<DMatrixRMaj> R_llt) {
    // Implements SDA algorithm on p. 5 of [1] (initial A, G, H are from (4)).
    //
    // [1] E. K.-W. Chu, H.-Y. Fan, W.-W. Lin & C.-S. Wang "Structure-Preserving
    //     Algorithms for Periodic Discrete-Time Algebraic Riccati Equations",
    //     International Journal of Control, 77:8, 767-788, 2004.
    //     DOI: 10.1080/00207170410001714988

    // A₀ = A
    // G₀ = BR⁻¹Bᵀ
    // H₀ = Q
    var A_k = A;
    var Rinv_BT = new SimpleMatrix(A.getNumRows(), A.getNumCols());
    R_llt.solve(B.transpose().getDDRM(), Rinv_BT.getDDRM());
    var G_k = B.mult(Rinv_BT);
    SimpleMatrix H_k;
    var H_k1 = Q;

    var W_solver = LinearSolverFactory_DDRM.lu(A.getNumRows());
    var V_1 = new SimpleMatrix(A.getNumRows(), A.getNumCols());
    var V_2 = new SimpleMatrix(A.getNumRows(), A.getNumCols());

    do {
      H_k = H_k1;

      // W = I + GₖHₖ
      var W = SimpleMatrix.identity(H_k.getNumRows()).plus(G_k.mult(H_k));

      W_solver.setA(W.getDDRM());

      // Solve WV₁ = Aₖ for V₁
      W_solver.solve(A_k.getDDRM(), V_1.getDDRM());

      // Solve V₂Wᵀ = Gₖ for V₂
      //
      // V₂Wᵀ = Gₖ
      // V₂ = Gₖ / Wᵀ
      // V₂ = (W \ Gₖᵀ)ᵀ
      // V₂ = W \ Gₖ since Gₖ and V₂ are symmetric
      W_solver.solve(G_k.getDDRM(), V_2.getDDRM());

      // Gₖ₊₁ = Gₖ + AₖV₂Aₖᵀ
      // Hₖ₊₁ = Hₖ + V₁ᵀHₖAₖ
      // Aₖ₊₁ = AₖV₁
      G_k = G_k.plus(A_k.mult(V_2).mult(A_k.transpose()));
      H_k1 = H_k.plus(V_1.transpose().mult(H_k).mult(A_k));
      A_k = A_k.mult(V_1);

      // while |Hₖ₊₁ − Hₖ| > ε |Hₖ₊₁|
    } while (H_k1.minus(H_k).normF() > 1e-10 * H_k1.normF());

    return new Matrix<>(H_k1);
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
    var R_llt = new LinearSolverSafe<DMatrixRMaj>(LinearSolverFactory_DDRM.chol(R.getNumRows()));
    R_llt.setA(R.getStorage().getDDRM());
    return dareDetail(A.getStorage(), B.getStorage(), Q.getStorage(), R_llt);
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
    var R_llt = new LinearSolverSafe<DMatrixRMaj>(LinearSolverFactory_DDRM.chol(R.getNumRows()));
    R_llt.setA(R.getStorage().getDDRM());
    var Rinv_NT = new SimpleMatrix(A.getNumRows(), A.getNumCols());
    R_llt.solve(B.getStorage().transpose().getDDRM(), Rinv_NT.getDDRM());

    // This is a change of variables to make the DARE that includes Q, R, and N
    // cost matrices fit the form of the DARE that includes only Q and R cost
    // matrices.
    //
    // This is equivalent to solving the original DARE:
    //
    //   A₂ᵀXA₂ − X − A₂ᵀXB(BᵀXB + R)⁻¹BᵀXA₂ + Q₂ = 0
    //
    // where A₂ and Q₂ are a change of variables:
    //
    //   A₂ = A − BR⁻¹Nᵀ and Q₂ = Q − NR⁻¹Nᵀ
    R_llt.solve(N.getStorage().transpose().getDDRM(), Rinv_NT.getDDRM());
    var A_2 = A.getStorage().minus(B.getStorage().mult(Rinv_NT));
    var Q_2 = Q.getStorage().minus(N.getStorage().mult(Rinv_NT));

    return dareDetail(A_2, B.getStorage(), Q_2, R_llt);
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
    // Require R be symmetric
    if (R.minus(R.transpose()).normF() > 1e-10) {
      throw new IllegalArgumentException("R was not symmetric.\n\n" + R);
    }

    // Require R be positive definite
    var R_llt = new LinearSolverSafe<DMatrixRMaj>(LinearSolverFactory_DDRM.chol(R.getNumRows()));
    if (!R_llt.setA(R.getStorage().getDDRM())) {
      throw new IllegalArgumentException("R was not positive definite.\n\n" + R);
    }

    // Require Q be symmetric
    if (Q.minus(Q.transpose()).normF() > 1e-10) {
      throw new IllegalArgumentException("Q was not symmetric.\n\n" + Q);
    }

    // Require Q be positive semidefinite
    var Q_es = Q.getStorage().eig();
    for (var e : Q_es.getEigenvalues()) {
      if (e.real < 0.0) {
        throw new IllegalArgumentException("Q was not positive semidefinite.\n\n" + Q);
      }
    }

    // Require (A, B) pair be stabilizable
    if (!LinearSystemUtil.isStabilizable(A, B)) {
      throw new IllegalArgumentException("(A, B) pair was not stabilizable.\n\n" + A + "\n" + B);
    }

    // Find C in Q = CᵀC
    var C = new SimpleMatrix(Q.getNumRows(), Q.getNumCols());
    for (int row = 0; row < Q.getNumRows(); ++row) {
      var e = Q_es.getEigenvalue(row);
      var v = Q_es.getEigenVector(row);
      if (v == null) {
        throw new IllegalArgumentException("Q was not positive semidefinite.\n\n" + Q);
      }
      for (int col = 0; col < Q.getNumRows(); ++col) {
        // We use e.real because the eigenvalues of a real symmetric matrix are
        // always real
        C.set(row, col, Math.sqrt(e.real) * v.get(col, 0));
      }
    }

    // Require (A, C) pair be detectable where Q = CᵀC
    if (!LinearSystemUtil.isDetectable(A, new Matrix<>(C))) {
      throw new IllegalArgumentException(
          "(A, C) pair where Q = CᵀC was not detectable.\n\n" + A + "\n" + C);
    }

    return dareDetail(A.getStorage(), B.getStorage(), Q.getStorage(), R_llt);
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
    // Require R be symmetric
    if (R.minus(R.transpose()).normF() > 1e-10) {
      throw new IllegalArgumentException("R was not symmetric.\n\n" + R);
    }

    // Require R be positive definite
    var R_llt = new LinearSolverSafe<DMatrixRMaj>(LinearSolverFactory_DDRM.chol(R.getNumRows()));
    if (!R_llt.setA(R.getStorage().getDDRM())) {
      throw new IllegalArgumentException("R was not positive definite.\n\n" + R);
    }

    // This is a change of variables to make the DARE that includes Q, R, and N
    // cost matrices fit the form of the DARE that includes only Q and R cost
    // matrices.
    //
    // This is equivalent to solving the original DARE:
    //
    //   A₂ᵀXA₂ − X − A₂ᵀXB(BᵀXB + R)⁻¹BᵀXA₂ + Q₂ = 0
    //
    // where A₂ and Q₂ are a change of variables:
    //
    //   A₂ = A − BR⁻¹Nᵀ and Q₂ = Q − NR⁻¹Nᵀ
    var Rinv_NT = new SimpleMatrix(R.getNumRows(), R.getNumCols());
    R_llt.solve(N.transpose().getStorage().getDDRM(), Rinv_NT.getDDRM());
    var Q_2 = Q.getStorage().minus(N.getStorage().mult(Rinv_NT));

    // Require Q be symmetric
    if (Q_2.minus(Q_2.transpose()).normF() > 1e-10) {
      throw new IllegalArgumentException("Q was not symmetric.\n\n" + Q_2);
    }

    // Require Q be positive semidefinite
    var Q_2_es = Q_2.eig();
    for (var e : Q_2_es.getEigenvalues()) {
      if (e.real < 0.0) {
        throw new IllegalArgumentException("Q was not positive semidefinite.\n\n" + Q_2);
      }
    }

    // Require (A, B) pair be stabilizable
    var A_2 = A.getStorage().minus(B.getStorage().mult(Rinv_NT));
    if (!LinearSystemUtil.isStabilizable(new Matrix<>(A_2), B)) {
      throw new IllegalArgumentException("(A, B) pair was not stabilizable.\n\n" + A_2 + "\n" + B);
    }

    // Find C in Q = CᵀC
    var C_2 = new SimpleMatrix(Q_2.getNumRows(), Q_2.getNumCols());
    for (int row = 0; row < Q_2.getNumRows(); ++row) {
      var e = Q_2_es.getEigenvalue(row);
      var v = Q_2_es.getEigenVector(row);
      if (v == null) {
        throw new IllegalArgumentException("Q was not positive semidefinite.\n\n" + Q_2);
      }
      for (int col = 0; col < Q_2.getNumRows(); ++col) {
        // We use e.real because the eigenvalues of a real symmetric matrix are
        // always real
        C_2.set(row, col, Math.sqrt(e.real) * v.get(col, 0));
      }
    }

    // Require (A, C) pair be detectable where Q = CᵀC
    if (!LinearSystemUtil.isDetectable(new Matrix<>(A_2), new Matrix<>(C_2))) {
      throw new IllegalArgumentException(
          "(A, C) pair where Q = CᵀC was not detectable.\n\n" + A_2 + "\n" + C_2);
    }

    return dareDetail(A_2, B.getStorage(), Q_2, R_llt);
  }
}
