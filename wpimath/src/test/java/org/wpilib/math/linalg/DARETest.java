// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.linalg;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;
import org.wpilib.UtilityClassTest;
import org.wpilib.math.numbers.N10;
import org.wpilib.math.numbers.N4;
import org.wpilib.math.util.Nat;
import org.wpilib.math.util.Num;

class DARETest extends UtilityClassTest<DARE> {
  private static final double DT = 1e-4;
  private static final int SCALE_EXPONENT = 44;

  DARETest() {
    super(DARE.class);
  }

  public static <R extends Num, C extends Num> void assertMatrixEqual(
      Matrix<R, C> A, Matrix<R, C> B) {
    for (int i = 0; i < A.getNumRows(); i++) {
      for (int j = 0; j < A.getNumCols(); j++) {
        assertEquals(A.get(i, j), B.get(i, j), 1e-4);
      }
    }
  }

  public static void assertAllFinite(Matrix<?, ?> matrix) {
    for (int row = 0; row < matrix.getNumRows(); row++) {
      for (int col = 0; col < matrix.getNumCols(); col++) {
        assertTrue(Double.isFinite(matrix.get(row, col)));
      }
    }
  }

  static Matrix<N10, N10> makeQuadcopterA() {
    var A = new Matrix<N10, N10>(Nat.N10(), Nat.N10());
    A.set(0, 0, 0.9999);
    A.set(0, 3, 1e-8);

    double stable = Math.exp(-DT);
    double unstable = Math.exp(6.0 * DT);
    A.set(3, 3, stable);
    A.set(4, 4, stable);
    A.set(5, 5, stable);
    A.set(6, 6, unstable);
    A.set(7, 7, unstable);
    A.set(8, 8, unstable);
    A.set(9, 9, unstable);

    return A;
  }

  static Matrix<N10, N4> makeQuadcopterB() {
    var B = new Matrix<N10, N4>(Nat.N10(), Nat.N4());
    double stableInput = 1.0 - Math.exp(-DT);
    double unstableInput = (Math.exp(6.0 * DT) - 1.0) / 6.0;
    for (int col = 0; col < B.getNumCols(); col++) {
      B.set(3, col, stableInput);
    }
    B.set(6, 0, unstableInput);
    B.set(7, 1, unstableInput);
    B.set(8, 2, unstableInput);
    B.set(9, 3, unstableInput);

    return B;
  }

  static Matrix<N10, N10> makeStateTransform() {
    var C = Matrix.eye(Nat.N10());
    C.set(0, 0, Math.scalb(1.0, SCALE_EXPONENT));
    return C;
  }

  static QuadcopterDAREProblem makeCoordinateTransformedQuadcopterDAREProblem() {
    var A = makeQuadcopterA();
    var B = makeQuadcopterB();
    var Q = Matrix.eye(Nat.N10());
    var R = Matrix.eye(Nat.N4());

    var C = makeStateTransform();
    var CInv = Matrix.eye(Nat.N10());
    CInv.set(0, 0, Math.scalb(1.0, -SCALE_EXPONENT));

    // Q = I keeps the deliberately unstable modes detectable. The 2^44 state
    // transform recreates the badly scaled repro using exactly representable
    // powers of two.
    return new QuadcopterDAREProblem(
        CInv.times(A).times(C), CInv.times(B), C.transpose().times(Q).times(C), R);
  }

  private static final class QuadcopterDAREProblem {
    final Matrix<N10, N10> A;
    final Matrix<N10, N4> B;
    final Matrix<N10, N10> Q;
    final Matrix<N4, N4> R;

    QuadcopterDAREProblem(
        Matrix<N10, N10> A, Matrix<N10, N4> B, Matrix<N10, N10> Q, Matrix<N4, N4> R) {
      this.A = A;
      this.B = B;
      this.Q = Q;
      this.R = R;
    }
  }

  <States extends Num, Inputs extends Num> double dareNormalizedResidual(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<States, States> Q,
      Matrix<Inputs, Inputs> R,
      Matrix<States, States> X) {
    var stateTerm = A.transpose().times(X).times(A);
    var inputCost = B.transpose().times(X).times(B).plus(R);
    var stateInput = A.transpose().times(X).times(B);
    var feedbackTerm = stateInput.times(inputCost.inv()).times(stateInput.transpose());
    var residual = stateTerm.minus(X).minus(feedbackTerm).plus(Q);
    double normalizer =
        Math.max(
            1.0,
            Math.max(
                Math.max(stateTerm.normF(), X.normF()), Math.max(feedbackTerm.normF(), Q.normF())));

    return residual.normF() / normalizer;
  }

  <States extends Num, Inputs extends Num> void assertDARESolutions(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<States, States> Q,
      Matrix<Inputs, Inputs> R) {
    assertDARESolution(A, B, Q, R, DARE.dare(A, B, Q, R), 1e-12);
    assertDARESolution(A, B, Q, R, DARE.dareNoPrecond(A, B, Q, R), 1e-12);
  }

  <States extends Num, Inputs extends Num> void assertDARESolution(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<States, States> Q,
      Matrix<Inputs, Inputs> R,
      Matrix<States, States> X) {
    assertDARESolution(A, B, Q, R, X, 1e-10);
  }

  <States extends Num, Inputs extends Num> void assertDARESolution(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<States, States> Q,
      Matrix<Inputs, Inputs> R,
      Matrix<States, States> X,
      double tolerance) {
    assertAllFinite(X);

    // Check that X is the solution to the DARE
    // Y = AᵀXA − X − AᵀXB(BᵀXB + R)⁻¹BᵀXA + Q = 0
    assertTrue(dareNormalizedResidual(A, B, Q, R, X) < tolerance);
  }

  <States extends Num, Inputs extends Num> void assertDARESolution(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<States, States> Q,
      Matrix<Inputs, Inputs> R,
      Matrix<States, Inputs> N,
      Matrix<States, States> X) {
    assertDARESolution(A, B, Q, R, N, X, 1e-10);
  }

  <States extends Num, Inputs extends Num> void assertDARESolution(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<States, States> Q,
      Matrix<Inputs, Inputs> R,
      Matrix<States, Inputs> N,
      Matrix<States, States> X,
      double tolerance) {
    assertAllFinite(X);

    // Check that X is the solution to the DARE
    // Y = AᵀXA − X − (AᵀXB + N)(BᵀXB + R)⁻¹(BᵀXA + Nᵀ) + Q = 0
    var stateTerm = A.transpose().times(X).times(A);
    var inputCost = B.transpose().times(X).times(B).plus(R);
    var leftTerm = A.transpose().times(X).times(B).plus(N);
    var rightTerm = B.transpose().times(X).times(A).plus(N.transpose());
    var feedbackTerm = leftTerm.times(inputCost.inv()).times(rightTerm);
    var residual = stateTerm.minus(X).minus(feedbackTerm).plus(Q);
    double normalizer =
        Math.max(
            1.0,
            Math.max(
                Math.max(stateTerm.normF(), X.normF()), Math.max(feedbackTerm.normF(), Q.normF())));

    assertTrue(residual.normF() / normalizer < tolerance);
  }

  @Test
  void testNonInvertibleA_ABQR() {
    // Example 2 of "On the Numerical Solution of the Discrete-Time Algebraic
    // Riccati Equation"

    var A =
        new Matrix<>(
            Nat.N4(), Nat.N4(), new double[] {0.5, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0});
    var B = new Matrix<>(Nat.N4(), Nat.N1(), new double[] {0, 0, 0, 1});
    var Q =
        new Matrix<>(
            Nat.N4(), Nat.N4(), new double[] {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    var R = new Matrix<>(Nat.N1(), Nat.N1(), new double[] {0.25});

    var X = DARE.dare(A, B, Q, R);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, X);
  }

  @Test
  void testNonInvertibleA_ABQRN() {
    // Example 2 of "On the Numerical Solution of the Discrete-Time Algebraic
    // Riccati Equation"

    var A =
        new Matrix<>(
            Nat.N4(), Nat.N4(), new double[] {0.5, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0});
    var B = new Matrix<>(Nat.N4(), Nat.N1(), new double[] {0, 0, 0, 1});
    var Q =
        new Matrix<>(
            Nat.N4(), Nat.N4(), new double[] {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    var R = new Matrix<>(Nat.N1(), Nat.N1(), new double[] {0.25});

    var Aref =
        new Matrix<>(
            Nat.N4(), Nat.N4(), new double[] {0.25, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0});
    Q = A.minus(Aref).transpose().times(Q).times(A.minus(Aref));
    R = B.transpose().times(Q).times(B).plus(R);
    var N = A.minus(Aref).transpose().times(Q).times(B);

    var X = DARE.dare(A, B, Q, R, N);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, N, X);
  }

  @Test
  void testInvertibleA_ABQR() {
    var A = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1, 1, 0, 1});
    var B = new Matrix<>(Nat.N2(), Nat.N1(), new double[] {0, 1});
    var Q = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1, 0, 0, 0});
    var R = new Matrix<>(Nat.N1(), Nat.N1(), new double[] {0.3});

    var X = DARE.dare(A, B, Q, R);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, X);
  }

  @Test
  void testInvertibleA_ABQRN() {
    var A = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1, 1, 0, 1});
    var B = new Matrix<>(Nat.N2(), Nat.N1(), new double[] {0, 1});
    var Q = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1, 0, 0, 0});
    var R = new Matrix<>(Nat.N1(), Nat.N1(), new double[] {0.3});

    var Aref = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {0.5, 1, 0, 1});
    Q = A.minus(Aref).transpose().times(Q).times(A.minus(Aref));
    R = B.transpose().times(Q).times(B).plus(R);
    var N = A.minus(Aref).transpose().times(Q).times(B);

    var X = DARE.dare(A, B, Q, R, N);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, N, X);
  }

  @Test
  void testFirstGeneralizedEigenvalueOfSTIsStable_ABQR() {
    // The first generalized eigenvalue of (S, T) is stable

    var A = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {0, 1, 0, 0});
    var B = new Matrix<>(Nat.N2(), Nat.N1(), new double[] {0, 1});
    var Q = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1, 0, 0, 1});
    var R = new Matrix<>(Nat.N1(), Nat.N1(), new double[] {1});

    var X = DARE.dare(A, B, Q, R);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, X);
  }

  @Test
  void testFirstGeneralizedEigenvalueOfSTIsStable_ABQRN() {
    // The first generalized eigenvalue of (S, T) is stable

    var A = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {0, 1, 0, 0});
    var B = new Matrix<>(Nat.N2(), Nat.N1(), new double[] {0, 1});
    var Q = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1, 0, 0, 1});
    var R = new Matrix<>(Nat.N1(), Nat.N1(), new double[] {1});

    var Aref = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {0, 0.5, 0, 0});
    Q = A.minus(Aref).transpose().times(Q).times(A.minus(Aref));
    R = B.transpose().times(Q).times(B).plus(R);
    var N = A.minus(Aref).transpose().times(Q).times(B);

    var X = DARE.dare(A, B, Q, R, N);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, N, X);
  }

  @Test
  void testIdentitySystem_ABQR() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = Matrix.eye(Nat.N2());
    var R = Matrix.eye(Nat.N2());

    var X = DARE.dare(A, B, Q, R);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, X);
  }

  @Test
  void testIdentitySystem_ABQRN() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = Matrix.eye(Nat.N2());
    var R = Matrix.eye(Nat.N2());
    var N = Matrix.eye(Nat.N2());

    var X = DARE.dare(A, B, Q, R, N);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, N, X);
  }

  @Test
  void testMoreInputsThanStates_ABQR() {
    var A = Matrix.eye(Nat.N2());
    var B = new Matrix<>(Nat.N2(), Nat.N3(), new double[] {1, 0, 0, 0, 0.5, 0.3});
    var Q = Matrix.eye(Nat.N2());
    var R = Matrix.eye(Nat.N3());

    var X = DARE.dare(A, B, Q, R);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, X);
  }

  @Test
  void testMoreInputsThanStates_ABQRN() {
    var A = Matrix.eye(Nat.N2());
    var B = new Matrix<>(Nat.N2(), Nat.N3(), new double[] {1, 0, 0, 0, 0.5, 0.3});
    var Q = Matrix.eye(Nat.N2());
    var R = Matrix.eye(Nat.N3());
    var N = new Matrix<>(Nat.N2(), Nat.N3(), new double[] {1, 0, 0, 0, 1, 0});

    var X = DARE.dare(A, B, Q, R, N);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, N, X);
  }

  @Test
  void testCoordinateTransformedQuadcopterDynamicJNI() {
    var problem = makeCoordinateTransformedQuadcopterDAREProblem();

    assertDARESolutions(problem.A, problem.B, problem.Q, problem.R);
  }

  @Test
  void testQNotSymmetric_ABQR() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1.0, 1.0, 0.0, 1.0});
    var R = Matrix.eye(Nat.N2());

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R));
  }

  @Test
  void testQNotPositiveSemidefinite_ABQR() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {-1.0, 0.0, 0.0, -1.0});
    var R = Matrix.eye(Nat.N2());

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R));
  }

  @Test
  void testQNotSymmetric_ABQRN() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1.0, 1.0, 0.0, 1.0});
    var R = Matrix.eye(Nat.N2());
    var N = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {2.0, 0.0, 0.0, 2.0});

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R, N));
  }

  @Test
  void testQNotPositiveSemidefinite_ABQRN() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = Matrix.eye(Nat.N2());
    var R = Matrix.eye(Nat.N2());
    var N = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {2.0, 0.0, 0.0, 2.0});

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R, N));
  }

  @Test
  void testRNotSymmetric_ABQR() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = Matrix.eye(Nat.N2());
    var R = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1.0, 1.0, 0.0, 1.0});

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R));
  }

  @Test
  void testRNotPositiveDefinite_ABQR() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = Matrix.eye(Nat.N2());

    var R1 = new Matrix<>(Nat.N2(), Nat.N2());
    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R1));

    var R2 = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {-1.0, 0.0, 0.0, -1.0});
    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R2));
  }

  @Test
  void testRNotSymmetric_ABQRN() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = Matrix.eye(Nat.N2());
    var N = Matrix.eye(Nat.N2());
    var R = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1.0, 1.0, 0.0, 1.0});

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R, N));
  }

  @Test
  void testRNotPositiveDefinite_ABQRN() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = Matrix.eye(Nat.N2());
    var N = Matrix.eye(Nat.N2());

    var R1 = new Matrix<>(Nat.N2(), Nat.N2());
    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R1, N));

    var R2 = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {-1.0, 0.0, 0.0, -1.0});
    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R2, N));
  }

  @Test
  void testABNotStabilizable_ABQR() {
    var A = Matrix.eye(Nat.N2());
    var B = new Matrix<>(Nat.N2(), Nat.N2());
    var Q = Matrix.eye(Nat.N2());
    var R = Matrix.eye(Nat.N2());

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R));
  }

  @Test
  void testABNotStabilizable_ABQRN() {
    var A = Matrix.eye(Nat.N2());
    var B = new Matrix<>(Nat.N2(), Nat.N2());
    var Q = Matrix.eye(Nat.N2());
    var R = Matrix.eye(Nat.N2());
    var N = Matrix.eye(Nat.N2());

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R, N));
  }

  @Test
  void testACNotDetectable_ABQR() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = new Matrix<>(Nat.N2(), Nat.N2());
    var R = Matrix.eye(Nat.N2());

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R));
  }

  @Test
  void testACNotDetectable_ABQRN() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = new Matrix<>(Nat.N2(), Nat.N2());
    var R = Matrix.eye(Nat.N2());
    var N = new Matrix<>(Nat.N2(), Nat.N2());

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R, N));
  }

  @Test
  void testQDecomposition() {
    // Ensures the decomposition of Q into CᵀC is correct

    var A = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1.0, 0.0, 0.0, 0.0});
    var B = Matrix.eye(Nat.N2());
    var R = Matrix.eye(Nat.N2());

    // (A, C₁) should be detectable pair
    var C_1 = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {0.0, 0.0, 1.0, 0.0});
    var Q_1 = C_1.transpose().times(C_1);
    assertDoesNotThrow(() -> DARE.dare(A, B, Q_1, R));

    // (A, C₂) shouldn't be detectable pair
    var C_2 = C_1.transpose();
    var Q_2 = C_2.transpose().times(C_2);
    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q_2, R));
  }
}
