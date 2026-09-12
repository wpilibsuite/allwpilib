// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.system;

import org.ejml.dense.row.factory.DecompositionFactory_DDRM;
import org.ejml.simple.SimpleMatrix;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.util.Num;

/** Linear system utilities. */
public final class LinearSystemUtil {
  private LinearSystemUtil() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Returns true if (A, B) is a stabilizable pair.
   *
   * <p>(A, B) is stabilizable if and only if the uncontrollable eigenvalues of A, if any, have
   * absolute values less than one, where an eigenvalue is uncontrollable if rank([λI - A, B]) &lt;
   * n where n is the number of states.
   *
   * @param <States> Num representing the size of A.
   * @param <Inputs> Num representing the columns of B.
   * @param A System matrix.
   * @param B Input matrix.
   * @return If the system is stabilizable.
   */
  public static <States extends Num, Inputs extends Num> boolean isStabilizable(
      Matrix<States, States> A, Matrix<States, Inputs> B) {
    var es = A.getStorage().eig();

    for (int i = 0; i < A.getNumRows(); ++i) {
      var e = es.getEigenvalue(i);
      if (e.getMagnitude() < 1) {
        continue;
      }

      // We need to get the rank of a complex matrix, but EJML doesn't have a
      // complex pivoting QR decomposition. Instead, we'll represent the complex
      // matrix as a real matrix with twice as many rows and columns.
      //
      //   E = [λI - A, B] ∈ ℂᴹˣᴺ
      //
      //           [Eᵣ  -Eᵢ]
      //   E_aug = [Eᵢ   Eᵣ] ∈ ℝ²ᴹˣ²ᴺ
      //
      // See https://math.stackexchange.com/a/2012242 for why this works.
      var E = new SimpleMatrix(A.getNumRows(), A.getNumCols() + B.getNumCols());
      E.insertIntoThis(
          0,
          0,
          SimpleMatrix.identity(A.getNumRows())
              .scaleComplex(e.real, e.imaginary)
              .minus(A.getStorage()));
      E.insertIntoThis(0, A.getNumCols(), B.getStorage());
      var E_r = E.real();
      var E_i = E.imaginary();
      var E_aug = new SimpleMatrix(2 * E.getNumRows(), 2 * E.getNumCols());
      E_aug.insertIntoThis(0, 0, E_r);
      E_aug.insertIntoThis(0, E.getNumCols(), E_i.scale(-1));
      E_aug.insertIntoThis(E.getNumRows(), 0, E_i);
      E_aug.insertIntoThis(E.getNumRows(), E.getNumCols(), E_r);

      var qrp = DecompositionFactory_DDRM.qrp(E_aug.getNumRows(), E_aug.getNumCols());
      if (!qrp.decompose(E_aug.getDDRM()) || qrp.getRank() < 2 * A.getNumRows()) {
        return false;
      }
    }
    return true;
  }

  /**
   * Returns true if (A, C) is a detectable pair.
   *
   * <p>(A, C) is detectable if and only if the unobservable eigenvalues of A, if any, have absolute
   * values less than one, where an eigenvalue is unobservable if rank([λI - A; C]) &lt; n where n
   * is the number of states.
   *
   * @param <States> Num representing the size of A.
   * @param <Outputs> Num representing the rows of C.
   * @param A System matrix.
   * @param C Output matrix.
   * @return If the system is detectable.
   */
  public static <States extends Num, Outputs extends Num> boolean isDetectable(
      Matrix<States, States> A, Matrix<Outputs, States> C) {
    return isStabilizable(A.transpose(), C.transpose());
  }
}
