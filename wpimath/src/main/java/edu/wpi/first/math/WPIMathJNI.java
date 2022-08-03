// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import edu.wpi.first.util.RuntimeLoader;
import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;

public final class WPIMathJNI {
  static boolean libraryLoaded = false;
  static RuntimeLoader<WPIMathJNI> loader = null;

  static {
    if (Helper.getExtractOnStaticLoad()) {
      try {
        loader =
            new RuntimeLoader<>(
                "wpimathjni", RuntimeLoader.getDefaultExtractionRoot(), WPIMathJNI.class);
        loader.loadLibrary();
      } catch (IOException ex) {
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
    loader =
        new RuntimeLoader<>(
            "wpimathjni", RuntimeLoader.getDefaultExtractionRoot(), WPIMathJNI.class);
    loader.loadLibrary();
    libraryLoaded = true;
  }

  /**
   * Solves the discrete alegebraic Riccati equation.
   *
   * @param A Array containing elements of A in row-major order.
   * @param B Array containing elements of B in row-major order.
   * @param Q Array containing elements of Q in row-major order.
   * @param R Array containing elements of R in row-major order.
   * @param states Number of states in A matrix.
   * @param inputs Number of inputs in B matrix.
   * @param S Array storage for DARE solution.
   */
  public static native void discreteAlgebraicRiccatiEquation(
      double[] A, double[] B, double[] Q, double[] R, int states, int inputs, double[] S);

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
   * Returns true if (A, B) is a stabilizable pair.
   *
   * <p>(A, B) is stabilizable if and only if the uncontrollable eigenvalues of A, if any, have
   * absolute values less than one, where an eigenvalue is uncontrollable if rank(lambda * I - A, B)
   * &lt; n where n is the number of states.
   *
   * @param states the number of states of the system.
   * @param inputs the number of inputs to the system.
   * @param A System matrix.
   * @param B Input matrix.
   * @return If the system is stabilizable.
   */
  public static native boolean isStabilizable(int states, int inputs, double[] A, double[] B);

  /**
   * Loads a Pathweaver JSON.
   *
   * @param path The path to the JSON.
   * @return A double array with the trajectory states from the JSON.
   * @throws IOException if the JSON could not be read.
   */
  public static native double[] fromPathweaverJson(String path) throws IOException;

  /**
   * Converts a trajectory into a Pathweaver JSON and saves it.
   *
   * @param elements The elements of the trajectory.
   * @param path The location to save the JSON to.
   * @throws IOException if the JSON could not be written.
   */
  public static native void toPathweaverJson(double[] elements, String path) throws IOException;

  /**
   * Deserializes a trajectory JSON into a double[] of trajectory elements.
   *
   * @param json The JSON containing the serialized trajectory.
   * @return A double array with the trajectory states.
   */
  public static native double[] deserializeTrajectory(String json);

  /**
   * Serializes the trajectory into a JSON string.
   *
   * @param elements The elements of the trajectory.
   * @return A JSON containing the serialized trajectory.
   */
  public static native String serializeTrajectory(double[] elements);

  /**
   * Performs an inplace rank one update (or downdate) of an upper triangular Cholesky decomposition
   * matrix.
   *
   * @param mat Array of elements of the matrix to be updated.
   * @param lowerTriangular Whether or not mat is lower triangular.
   * @param rows How many rows there are.
   * @param vec Vector to use for the rank update.
   * @param sigma Sigma value to use for the rank update.
   */
  public static native void rankUpdate(
      double[] mat, int rows, double[] vec, double sigma, boolean lowerTriangular);

  public static class Helper {
    private static AtomicBoolean extractOnStaticLoad = new AtomicBoolean(true);

    public static boolean getExtractOnStaticLoad() {
      return extractOnStaticLoad.get();
    }

    public static void setExtractOnStaticLoad(boolean load) {
      extractOnStaticLoad.set(load);
    }
  }

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
}
