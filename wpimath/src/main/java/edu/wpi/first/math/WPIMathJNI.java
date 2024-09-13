// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import edu.wpi.first.util.RuntimeLoader;
import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;

/** WPIMath JNI. */
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

  // DARE wrappers

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
   * alegebraic Riccati equation.
   *
   * @param A Array containing elements of A in row-major order.
   * @param B Array containing elements of B in row-major order.
   * @param Q Array containing elements of Q in row-major order.
   * @param R Array containing elements of R in row-major order.
   * @param states Number of states in A matrix.
   * @param inputs Number of inputs in B matrix.
   * @param S Array storage for DARE solution.
   */
  public static native void dareDetailABQR(
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
  public static native void dareDetailABQRN(
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

  // Eigen wrappers

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

  // Pose3d wrappers

  /**
   * Obtain a Pose3d from a (constant curvature) velocity.
   *
   * <p>The double array returned is of the form [dx, dy, dz, qx, qy, qz].
   *
   * @param poseX The pose's translational X component.
   * @param poseY The pose's translational Y component.
   * @param poseZ The pose's translational Z component.
   * @param poseQw The pose quaternion's W component.
   * @param poseQx The pose quaternion's X component.
   * @param poseQy The pose quaternion's Y component.
   * @param poseQz The pose quaternion's Z component.
   * @param twistDx The twist's dx value.
   * @param twistDy The twist's dy value.
   * @param twistDz The twist's dz value.
   * @param twistRx The twist's rx value.
   * @param twistRy The twist's ry value.
   * @param twistRz The twist's rz value.
   * @return The new pose as a double array.
   */
  public static native double[] expPose3d(
      double poseX,
      double poseY,
      double poseZ,
      double poseQw,
      double poseQx,
      double poseQy,
      double poseQz,
      double twistDx,
      double twistDy,
      double twistDz,
      double twistRx,
      double twistRy,
      double twistRz);

  /**
   * Returns a Twist3d that maps the starting pose to the end pose.
   *
   * <p>The double array returned is of the form [dx, dy, dz, rx, ry, rz].
   *
   * @param startX The starting pose's translational X component.
   * @param startY The starting pose's translational Y component.
   * @param startZ The starting pose's translational Z component.
   * @param startQw The starting pose quaternion's W component.
   * @param startQx The starting pose quaternion's X component.
   * @param startQy The starting pose quaternion's Y component.
   * @param startQz The starting pose quaternion's Z component.
   * @param endX The ending pose's translational X component.
   * @param endY The ending pose's translational Y component.
   * @param endZ The ending pose's translational Z component.
   * @param endQw The ending pose quaternion's W component.
   * @param endQx The ending pose quaternion's X component.
   * @param endQy The ending pose quaternion's Y component.
   * @param endQz The ending pose quaternion's Z component.
   * @return The twist that maps start to end as a double array.
   */
  public static native double[] logPose3d(
      double startX,
      double startY,
      double startZ,
      double startQw,
      double startQx,
      double startQy,
      double startQz,
      double endX,
      double endY,
      double endZ,
      double endQw,
      double endQx,
      double endQy,
      double endQz);

  // StateSpaceUtil wrappers

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

  // Trajectory wrappers

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
  private WPIMathJNI() {}
}
