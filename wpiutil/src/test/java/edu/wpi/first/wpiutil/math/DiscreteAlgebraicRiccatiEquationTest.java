/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil.math;

import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

public class DiscreteAlgebraicRiccatiEquationTest {
    private void solveDAREandVerify(SimpleMatrix A, SimpleMatrix B, SimpleMatrix Q, SimpleMatrix R){
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
        A1 = new SimpleMatrix(n1, n1, true, new double[] {0.5, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0}).transpose();
        B1 = new SimpleMatrix(n1, m1, true, new double[] {0, 0, 0, 1});
        Q1 = new SimpleMatrix(n1, n1, true, new double[] {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
        R1 = new SimpleMatrix(m1, m1, true, new double[] {0.25});
        solveDAREandVerify(A1, B1, Q1, R1);

        //  B2 << 0, 1;
//  Q2 << 1, 0, 0, 0;
//  R2 << 0.3;

        SimpleMatrix A2, B2, Q2, R2;
        A2 = new SimpleMatrix(2, 2, true, new double[] { 1, 1, 0, 1 });
        B2 = new SimpleMatrix(2, 1, true, new double[] { 0, 1 });
        Q2 = new SimpleMatrix(2, 2, true, new double[] { 1, 0, 0, 0 });
        R2 = new SimpleMatrix(1, 1, true, new double[] { 0.3 });
        solveDAREandVerify(A2, B2, Q2, R2);


    }

    public static void assertMatrixEqual(SimpleMatrix A, SimpleMatrix B) {
        for(int i = 0; i < A.numRows(); i++) {
            for(int j = 0; j < A.numCols(); j++) {
                try {
                    assertEquals(A.get(i, j), B.get(i,j), 1e-4);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }

}

// #include "drake/math/discrete_algebraic_riccati_equation.h"
//
//#include <Eigen/Core>
//#include <Eigen/Eigenvalues>
//
//#include <gtest/gtest.h>
//
//#include <iostream>
//
//#include "drake/common/test_utilities/eigen_matrix_compare.h"
//#include "drake/math/autodiff.h"
//
//using Eigen::MatrixXd;
//
//namespace drake {
//namespace math {
//namespace {
//void SolveDAREandVerify(const Eigen::Ref<const MatrixXd>& A,
//                        const Eigen::Ref<const MatrixXd>& B,
//                        const Eigen::Ref<const MatrixXd>& Q,
//                        const Eigen::Ref<const MatrixXd>& R) {
//  MatrixXd X = DiscreteAlgebraicRiccatiEquation(A, B, Q, R);
//  // Check that X is positive semi-definite.
//  EXPECT_TRUE(
//      CompareMatrices(X, X.transpose(), 1E-10, MatrixCompareType::absolute));
//
//  int n = X.rows();
//  Eigen::SelfAdjointEigenSolver<MatrixXd> es(X);
//  for (int i = 0; i < n; i++) {
//    EXPECT_GE(es.eigenvalues()[i], 0);
//  }
//  // Check that X is the solution to the discrete time ARE.
//  MatrixXd Y = A.transpose() * X * A - X -
//               A.transpose() * X * B * (B.transpose() * X * B + R).inverse() *
//                   B.transpose() * X * A +
//               Q;
//
//  EXPECT_TRUE(CompareMatrices(Y, MatrixXd::Zero(n, n), 1E-10,
//                              MatrixCompareType::absolute));
//}
//
//GTEST_TEST(DARE, SolveDAREandVerify) {
//  // Test 1: non-invertible A
//  // Example 2 of "On the Numerical Solution of the Discrete-Time Algebraic
//  // Riccati Equation"
//  int n1 = 4, m1 = 1;
//  MatrixXd A1(n1, n1), B1(n1, m1), Q1(n1, n1), R1(m1, m1);
//  A1 << 0.5, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0;
//  B1 << 0, 0, 0, 1;
//  Q1 << 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
//  R1 << 0.25;
//  SolveDAREandVerify(A1, B1, Q1, R1);
//  // Test 2: invertible A
//  int n2 = 2, m2 = 1;
//  MatrixXd A2(n2, n2), B2(n2, m2), Q2(n2, n2), R2(m2, m2);
//  A2 << 1, 1, 0, 1;
//  B2 << 0, 1;
//  Q2 << 1, 0, 0, 0;
//  R2 << 0.3;
//  SolveDAREandVerify(A2, B2, Q2, R2);
//}
//}  // namespace
//}  // namespace math
//}  // namespace drake
