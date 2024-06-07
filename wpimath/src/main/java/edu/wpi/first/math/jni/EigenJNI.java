// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.jni;

import edu.wpi.first.util.RuntimeLoader;
import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;

/** Eigen JNI. */
public final class EigenJNI {
  static boolean libraryLoaded = false;

  static {
    if (Helper.getExtractOnStaticLoad()) {
      try {
        RuntimeLoader.loadLibrary("wpimathjni");
      } catch (Exception ex) {
        ex.printStackTrace();
        System.exit(1);
      }
      libraryLoaded = true;
    }
  }

  /**
   * Force load the library.
   *
   * @throws IOException If the library could not be loaded or found.
   */
  public static synchronized void forceLoad() throws IOException {
    if (libraryLoaded) {
      return;
    }
    RuntimeLoader.loadLibrary("wpimathjni");
    libraryLoaded = true;
  }

  /**
   * Computes the matrix exp.
   *
   * @param src Array of elements of the matrix to be exponentiated.
   * @param rows How many rows there are.
   * @param dst Array where the result will be stored.
   */
  public static native void exp(double[] src, int rows, double[] dst);

  /**
   * Computes the matrix pow.
   *
   * @param src Array of elements of the matrix to be raised to a power.
   * @param rows How many rows there are.
   * @param exponent The exponent.
   * @param dst Array where the result will be stored.
   */
  public static native void pow(double[] src, int rows, double exponent, double[] dst);

  /**
   * Performs an inplace rank one update (or downdate) of an upper triangular Cholesky decomposition
   * matrix.
   *
   * @param mat Array of elements of the matrix to be updated.
   * @param lowerTriangular Whether mat is lower triangular.
   * @param rows How many rows there are.
   * @param vec Vector to use for the rank update.
   * @param sigma Sigma value to use for the rank update.
   */
  public static native void rankUpdate(
      double[] mat, int rows, double[] vec, double sigma, boolean lowerTriangular);

  /**
   * Solves the least-squares problem Ax=B using a QR decomposition with full pivoting.
   *
   * @param A Array of elements of the A matrix.
   * @param Arows Number of rows of the A matrix.
   * @param Acols Number of rows of the A matrix.
   * @param B Array of elements of the B matrix.
   * @param Brows Number of rows of the B matrix.
   * @param Bcols Number of rows of the B matrix.
   * @param dst Array to store solution in. If A is m-n and B is m-p, dst is n-p.
   */
  public static native void solveFullPivHouseholderQr(
      double[] A, int Arows, int Acols, double[] B, int Brows, int Bcols, double[] dst);

  /** Sets whether JNI should be loaded in the static block. */
  public static class Helper {
    private static AtomicBoolean extractOnStaticLoad = new AtomicBoolean(true);

    /**
     * Returns true if the JNI should be loaded in the static block.
     *
     * @return True if the JNI should be loaded in the static block.
     */
    public static boolean getExtractOnStaticLoad() {
      return extractOnStaticLoad.get();
    }

    /**
     * Sets whether the JNI should be loaded in the static block.
     *
     * @param load Whether the JNI should be loaded in the static block.
     */
    public static void setExtractOnStaticLoad(boolean load) {
      extractOnStaticLoad.set(load);
    }

    /** Utility class. */
    private Helper() {}
  }

  /** Utility class. */
  private EigenJNI() {}
}
