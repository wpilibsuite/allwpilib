package edu.wpi.first.wpiutil.math;

import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

public class DiscreteAlgebraicRiccatiEquationTest {

  public static void assertMatrixEqual(SimpleMatrix A, SimpleMatrix B) {
    for (int i = 0; i < A.numRows(); i++) {
      for (int j = 0; j < A.numCols(); j++) {
        try {
          assertEquals(A.get(i, j), B.get(i, j), 1e-4);
        } catch (Exception e) {
          e.printStackTrace();
        }
      }
    }
  }

  private void solveDAREandVerify(SimpleMatrix A, SimpleMatrix B, SimpleMatrix Q, SimpleMatrix R) {
    var X = Drake.discreteAlgebraicRiccatiEquation(A, B, Q, R);

    // expect that x is the same as it's transpose
    assertEquals(X.numRows(), X.numCols());
    assertMatrixEqual(X, X.transpose());

    // Verify that this is a solution to the DARE.
    SimpleMatrix Y = A.transpose().mult(X).mult(A)
            .minus(X)
            .minus(A.transpose().mult(X).mult(B)
                    .mult(((B.transpose().mult(X).mult(B)).plus(R)).invert()).mult(B.transpose()).mult(X).mult(A))
            .plus(Q);
    assertMatrixEqual(Y, new SimpleMatrix(Y.numRows(), Y.numCols()));
  }

  @Test
  public void testDARE() {
    int n1 = 4, m1 = 1;

    // we know from Scipy that this should be [[0.05048525 0.10097051 0.20194102 0.40388203]]
    SimpleMatrix A1, B1, Q1, R1;
    A1 = new SimpleMatrix(n1, n1, true, new double[]{0.5, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0}).transpose();
    B1 = new SimpleMatrix(n1, m1, true, new double[]{0, 0, 0, 1});
    Q1 = new SimpleMatrix(n1, n1, true, new double[]{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    R1 = new SimpleMatrix(m1, m1, true, new double[]{0.25});
    solveDAREandVerify(A1, B1, Q1, R1);

    //  B2 << 0, 1;
//  Q2 << 1, 0, 0, 0;
//  R2 << 0.3;

    SimpleMatrix A2, B2, Q2, R2;
    A2 = new SimpleMatrix(2, 2, true, new double[]{1, 1, 0, 1});
    B2 = new SimpleMatrix(2, 1, true, new double[]{0, 1});
    Q2 = new SimpleMatrix(2, 2, true, new double[]{1, 0, 0, 0});
    R2 = new SimpleMatrix(1, 1, true, new double[]{0.3});
    solveDAREandVerify(A2, B2, Q2, R2);


  }
}