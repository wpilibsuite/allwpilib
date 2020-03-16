/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil.math;

import org.ejml.data.DMatrixRMaj;
import org.ejml.dense.row.NormOps_DDRM;
import org.ejml.dense.row.decomposition.qr.QRDecompositionHouseholder_DDRM;
import org.ejml.dense.row.factory.DecompositionFactory_DDRM;
import org.ejml.interfaces.decomposition.CholeskyDecomposition_F64;
import org.ejml.simple.SimpleBase;
import org.ejml.simple.SimpleMatrix;

import java.util.function.BiFunction;

public class SimpleMatrixUtils {
  private SimpleMatrixUtils() {
  }

  /**
   * Compute the matrix exponential, e^M of the given matrix.
   *
   * @param matrix The matrix to compute the exponential of.
   * @return The resultant matrix.
   */
  @SuppressWarnings({"LocalVariableName", "LineLength"})
  public static SimpleMatrix expm(SimpleMatrix matrix) {
    BiFunction<SimpleMatrix, SimpleMatrix, SimpleMatrix> solveProvider = SimpleBase::solve;
    SimpleMatrix A = matrix;
    double A_L1 = NormOps_DDRM.inducedP1(matrix.getDDRM());
    int n_squarings = 0;

    if (A_L1 < 1.495585217958292e-002) {
      Pair<SimpleMatrix, SimpleMatrix> pair = _pade3(A);
      return dispatchPade(pair.getFirst(), pair.getSecond(), n_squarings, solveProvider);
    } else if (A_L1 < 2.539398330063230e-001) {
      Pair<SimpleMatrix, SimpleMatrix> pair = _pade5(A);
      return dispatchPade(pair.getFirst(), pair.getSecond(), n_squarings, solveProvider);
    } else if (A_L1 < 9.504178996162932e-001) {
      Pair<SimpleMatrix, SimpleMatrix> pair = _pade7(A);
      return dispatchPade(pair.getFirst(), pair.getSecond(), n_squarings, solveProvider);
    } else if (A_L1 < 2.097847961257068e+000) {
      Pair<SimpleMatrix, SimpleMatrix> pair = _pade9(A);
      return dispatchPade(pair.getFirst(), pair.getSecond(), n_squarings, solveProvider);
    } else {
      double maxNorm = 5.371920351148152;
      double log = Math.log(A_L1 / maxNorm) / Math.log(2); // logb(2, arg)
      n_squarings = (int) Math.max(0, Math.ceil(log));
      A = A.divide(Math.pow(2.0, n_squarings));
      Pair<SimpleMatrix, SimpleMatrix> pair = _pade13(A);
      return dispatchPade(pair.getFirst(), pair.getSecond(), n_squarings, solveProvider);
    }
  }

  @SuppressWarnings({"LocalVariableName", "ParameterName", "LineLength"})
  private static SimpleMatrix dispatchPade(SimpleMatrix U, SimpleMatrix V,
                                           int nSquarings, BiFunction<SimpleMatrix, SimpleMatrix, SimpleMatrix> solveProvider) {
    SimpleMatrix P = U.plus(V);
    SimpleMatrix Q = U.negative().plus(V);

    SimpleMatrix R = solveProvider.apply(Q, P);

    for (int i = 0; i < nSquarings; i++) {
      R = R.mult(R);
    }

    return R;
  }

  @SuppressWarnings({"MethodName", "LocalVariableName", "ParameterName"})
  private static Pair<SimpleMatrix, SimpleMatrix> _pade3(SimpleMatrix A) {
    double[] b = new double[]{120, 60, 12, 1};
    SimpleMatrix ident = eye(A.numRows(), A.numCols());

    SimpleMatrix A2 = A.mult(A);
    SimpleMatrix U = A.mult(A2.mult(ident.scale(b[1]).plus(b[3])));
    SimpleMatrix V = A2.scale(b[2]).plus(ident.scale(b[0]));
    return new Pair<>(U, V);
  }

  @SuppressWarnings({"MethodName", "LocalVariableName", "ParameterName"})
  private static Pair<SimpleMatrix, SimpleMatrix> _pade5(SimpleMatrix A) {
    double[] b = new double[]{30240, 15120, 3360, 420, 30, 1};
    SimpleMatrix ident = eye(A.numRows(), A.numCols());
    SimpleMatrix A2 = A.mult(A);
    SimpleMatrix A4 = A2.mult(A2);

    SimpleMatrix U = A.mult(A4.scale(b[5]).plus(A2.scale(b[3])).plus(ident.scale(b[1])));
    SimpleMatrix V = A4.scale(b[4]).plus(A2.scale(b[2])).plus(ident.scale(b[0]));

    return new Pair<>(U, V);
  }

  @SuppressWarnings({"MethodName", "LocalVariableName", "LineLength", "ParameterName"})
  private static Pair<SimpleMatrix, SimpleMatrix> _pade7(SimpleMatrix A) {
    double[] b = new double[]{17297280, 8648640, 1995840, 277200, 25200, 1512, 56, 1};
    SimpleMatrix ident = eye(A.numRows(), A.numCols());
    SimpleMatrix A2 = A.mult(A);
    SimpleMatrix A4 = A2.mult(A2);
    SimpleMatrix A6 = A4.mult(A2);

    SimpleMatrix U = A.mult(A6.scale(b[7]).plus(A4.scale(b[5])).plus(A2.scale(b[3])).plus(ident.scale(b[1])));
    SimpleMatrix V = A6.scale(b[6]).plus(A4.scale(b[4])).plus(A2.scale(b[2])).plus(ident.scale(b[0]));

    return new Pair<>(U, V);
  }

  @SuppressWarnings({"MethodName", "LocalVariableName", "ParameterName", "LineLength"})
  private static Pair<SimpleMatrix, SimpleMatrix> _pade9(SimpleMatrix A) {
    double[] b = new double[]{17643225600.0, 8821612800.0, 2075673600, 302702400, 30270240,
            2162160, 110880, 3960, 90, 1};
    SimpleMatrix ident = eye(A.numRows(), A.numCols());
    SimpleMatrix A2 = A.mult(A);
    SimpleMatrix A4 = A2.mult(A2);
    SimpleMatrix A6 = A4.mult(A2);
    SimpleMatrix A8 = A6.mult(A2);

    SimpleMatrix U = A.mult(A8.scale(b[9]).plus(A6.scale(b[7])).plus(A4.scale(b[5])).plus(A2.scale(b[3])).plus(ident.scale(b[1])));
    SimpleMatrix V = A8.scale(b[8]).plus(A6.scale(b[6])).plus(A4.scale(b[4])).plus(A2.scale(b[2])).plus(ident.scale(b[0]));

    return new Pair<>(U, V);
  }

  @SuppressWarnings({"MethodName", "LocalVariableName", "LineLength", "ParameterName"})
  private static Pair<SimpleMatrix, SimpleMatrix> _pade13(SimpleMatrix A) {
    double[] b = new double[]{64764752532480000.0, 32382376266240000.0, 7771770303897600.0,
            1187353796428800.0, 129060195264000.0, 10559470521600.0, 670442572800.0,
            33522128640.0, 1323241920, 40840800, 960960, 16380, 182, 1};
    SimpleMatrix ident = eye(A.numRows(), A.numCols());

    SimpleMatrix A2 = A.mult(A);
    SimpleMatrix A4 = A2.mult(A2);
    SimpleMatrix A6 = A4.mult(A2);

    SimpleMatrix U = A.mult(A6.scale(b[13]).plus(A4.scale(b[11])).plus(A2.scale(b[9])).plus(A6.scale(b[7])).plus(A4.scale(b[5])).plus(A2.scale(b[3])).plus(ident.scale(b[1])));
    SimpleMatrix V = A6.mult(A6.scale(b[12]).plus(A4.scale(b[10])).plus(A2.scale(b[8]))).plus(A6.scale(b[6]).plus(A4.scale(b[4])).plus(A2.scale(b[2])).plus(ident.scale(b[0])));

    return new Pair<>(U, V);
  }

  private static SimpleMatrix eye(int rows, int cols) {
    return SimpleMatrix.identity(Math.min(rows, cols));
  }

  /**
   * The identy of a square matrix
   *
   * @param rows the number of rows (and columns)
   * @return the identiy matrix, rows x rows.
   */
  private static SimpleMatrix eye(int rows) {
    return SimpleMatrix.identity(rows);
  }

  public static SimpleMatrix lltDecompose(SimpleMatrix src) {
    SimpleMatrix temp = src.copy();
    CholeskyDecomposition_F64<DMatrixRMaj> chol =
            DecompositionFactory_DDRM.chol(temp.numRows(), true);
    if (!chol.decompose(temp.getMatrix()))
      throw new RuntimeException("Cholesky failed!");

    return SimpleMatrix.wrap(chol.getT(null));
  }

  public static SimpleMatrix householderQrDecompose(SimpleMatrix src) {
    var temp = src.copy();

    var decomposer = new QRDecompositionHouseholder_DDRM();
    if (!(decomposer.decompose(temp.getDDRM()))) {
      throw new RuntimeException("householder decomposition failed!");
    }

    return temp;
  }

  public static class Pair<A, B> {
    private final A m_first;
    private final B m_second;

    public Pair(A first, B second) {
      m_first = first;
      m_second = second;
    }

    public A getFirst() {
      return m_first;
    }

    public B getSecond() {
      return m_second;
    }

    public static <A, B> Pair<A, B> of(A a, B b) {
      return new Pair<>(a, b);
    }
  }

  /**
   * Calculate matrix exponential of a square matrix.
   * <p>
   * Implementation from jblas https://github.com/jblas-project/jblas
   * <p>
   * A scaled Pade approximation algorithm is used.
   * The algorithm has been directly translated from Golub and Van Loan "Matrix Computations",
   * algorithm 11.3.1. Special Horner techniques from 11.2 are also used to minimize the number
   * of matrix multiplications.
   *
   * @param A a square matrix.
   * @return the matrix exponential of A.
   */
  public static SimpleMatrix scipyExpm(SimpleMatrix A) {

    /*
     * Copyright (c) 2009, Mikio L. Braun
     * All rights reserved.
     *
     * Redistribution and use in source and binary forms, with or without
     * modification, are permitted provided that the following conditions are
     * met:
     *
     *     * Redistributions of source code must retain the above copyright
     *       notice, this list of conditions and the following disclaimer.
     *
     *     * Redistributions in binary form must reproduce the above
     *       copyright notice, this list of conditions and the following
     *       disclaimer in the documentation and/or other materials provided
     *       with the distribution.
     *
     *     * Neither the name of the Technische Universität Berlin nor the
     *       names of its contributors may be used to endorse or promote
     *       products derived from this software without specific prior
     *       written permission.
     *
     * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
     * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
     * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
     * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
     * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
     * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
     * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
     * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
     * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
     * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
     * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
     */

    if (A.numRows() != A.numCols()) {
      throw new RuntimeException("A must be square to preform expm!");
    }

    // constants for pade approximation
    final double c0 = 1.0;
    final double c1 = 0.5;
    final double c2 = 0.12;
    final double c3 = 0.01833333333333333;
    final double c4 = 0.0019927536231884053;
    final double c5 = 1.630434782608695E-4;
    final double c6 = 1.0351966873706E-5;
    final double c7 = 5.175983436853E-7;
    final double c8 = 2.0431513566525E-8;
    final double c9 = 6.306022705717593E-10;
    final double c10 = 1.4837700484041396E-11;
    final double c11 = 2.5291534915979653E-13;
    final double c12 = 2.8101705462199615E-15;
    final double c13 = 1.5440497506703084E-17;

    int j = Math.max(0, 1 + (int) Math.floor(Math.log(A.elementMaxAbs()) / Math.log(2)));
    SimpleMatrix As = A.divide(Math.pow(2, j)); // scaled version of A
    int n = A.numRows();

    // calculate D and N using special Horner techniques
    SimpleMatrix As_2 = As.mult(As);
    SimpleMatrix As_4 = As_2.mult(As_2);
    SimpleMatrix As_6 = As_4.mult(As_2);
    // U = c0*I + c2*A^2 + c4*A^4 + (c6*I + c8*A^2 + c10*A^4 + c12*A^6)*A^6
    SimpleMatrix U = eye(n).scale(c0).plus(As_2.scale(c2)).plus(As_4.scale(c4)).plus(
            eye(n).scale(c6).plus(As_2.scale(c8)).plus(As_4.scale(c10)).plus(As_6.scale(c12)).mult(As_6));
    // V = c1*I + c3*A^2 + c5*A^4 + (c7*I + c9*A^2 + c11*A^4 + c13*A^6)*A^6
    SimpleMatrix V = eye(n).scale(c1).plus(As_2.scale(c3)).plus(As_4.scale(c5)).plus(
            eye(n).scale(c7).plus(As_2.scale(c9)).plus(As_4.scale(c11)).plus(As_6.scale(c13)).mult(As_6));

    SimpleMatrix AV = As.mult(V);
    SimpleMatrix N = U.plus(AV);
    SimpleMatrix D = U.minus(AV);

    // solve DF = N for F
    SimpleMatrix F = D.solve(N);

    // now square j times
    for (int k = 0; k < j; k++) {
      F.mult(F);
    }

    return F;

  }
}
