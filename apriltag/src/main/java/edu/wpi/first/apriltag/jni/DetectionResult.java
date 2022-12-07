// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag.jni;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Transform3d;
import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.math.numbers.N3;
import java.util.Arrays;
import org.ejml.data.DMatrixRMaj;
import org.ejml.dense.row.factory.DecompositionFactory_DDRM;
import org.ejml.simple.SimpleMatrix;

public class DetectionResult {
  public int getId() {
    return m_id;
  }

  public int getHamming() {
    return m_hamming;
  }

  public float getDecisionMargin() {
    return m_decisionMargin;
  }

  public void setDecisionMargin(float decisionMargin) {
    this.m_decisionMargin = decisionMargin;
  }

  @SuppressWarnings("PMD.MethodReturnsInternalArray")
  public double[] getHomography() {
    return m_homography;
  }

  @SuppressWarnings("PMD.ArrayIsStoredDirectly")
  public void setHomography(double[] homography) {
    this.m_homography = homography;
  }

  public double getCenterX() {
    return m_centerX;
  }

  public void setCenterX(double centerX) {
    this.m_centerX = centerX;
  }

  public double getCenterY() {
    return m_centerY;
  }

  public void setCenterY(double centerY) {
    this.m_centerY = centerY;
  }

  @SuppressWarnings("PMD.MethodReturnsInternalArray")
  public double[] getCorners() {
    return m_corners;
  }

  @SuppressWarnings("PMD.ArrayIsStoredDirectly")
  public void setCorners(double[] corners) {
    this.m_corners = corners;
  }

  public double getError1() {
    return m_error1;
  }

  public double getError2() {
    return m_error2;
  }

  public Transform3d getPoseResult1() {
    return m_poseResult1;
  }

  public Transform3d getPoseResult2() {
    return m_poseResult2;
  }

  private final int m_id;
  private final int m_hamming;
  private float m_decisionMargin;
  private double[] m_homography;
  private double m_centerX;
  private double m_centerY;
  private double[] m_corners;

  private final Transform3d m_poseResult1;
  private final double m_error1;
  private final Transform3d m_poseResult2;
  private final double m_error2;

  /**
   * Constructs a new detection result. Used from JNI.
   *
   * @param id id
   * @param hamming hamming
   * @param decisionMargin dm
   * @param homography homography
   * @param centerX centerX
   * @param centerY centerY
   * @param corners corners
   * @param pose1TransArr pose1TransArr
   * @param pose1RotArr pose1RotArr
   * @param err1 err1
   * @param pose2TransArr pose2TransArr
   * @param pose2RotArr pose2RotArr
   * @param err2 err2
   */
  @SuppressWarnings("PMD.ArrayIsStoredDirectly")
  public DetectionResult(
      int id,
      int hamming,
      float decisionMargin,
      double[] homography,
      double centerX,
      double centerY,
      double[] corners,
      double[] pose1TransArr,
      double[] pose1RotArr,
      double err1,
      double[] pose2TransArr,
      double[] pose2RotArr,
      double err2) {
    this.m_id = id;
    this.m_hamming = hamming;
    this.m_decisionMargin = decisionMargin;
    this.m_homography = homography;
    this.m_centerX = centerX;
    this.m_centerY = centerY;
    this.m_corners = corners;

    this.m_error1 = err1;
    this.m_poseResult1 =
        new Transform3d(
            new Translation3d(pose1TransArr[0], pose1TransArr[1], pose1TransArr[2]),
            new Rotation3d(
                orthogonalizeRotationMatrix(
                    new MatBuilder<>(Nat.N3(), Nat.N3()).fill(pose1RotArr))));
    this.m_error2 = err2;
    this.m_poseResult2 =
        new Transform3d(
            new Translation3d(pose2TransArr[0], pose2TransArr[1], pose2TransArr[2]),
            new Rotation3d(
                orthogonalizeRotationMatrix(
                    new MatBuilder<>(Nat.N3(), Nat.N3()).fill(pose2RotArr))));
  }

  /**
   * Get the ratio of pose reprojection errors, called ambiguity. Numbers above 0.2 are likely to be
   * ambiguous.
   *
   * @return The ratio of pose reprojection errors.
   */
  public double getPoseAmbiguity() {
    var min = Math.min(m_error1, m_error2);
    var max = Math.max(m_error1, m_error2);

    if (max > 0) {
      return min / max;
    } else {
      return -1;
    }
  }

  @Override
  public String toString() {
    return "DetectionResult [centerX="
        + m_centerX
        + ", centerY="
        + m_centerY
        + ", corners="
        + Arrays.toString(m_corners)
        + ", decisionMargin="
        + m_decisionMargin
        + ", error1="
        + m_error1
        + ", error2="
        + m_error2
        + ", hamming="
        + m_hamming
        + ", homography="
        + Arrays.toString(m_homography)
        + ", id="
        + m_id
        + ", poseResult1="
        + m_poseResult1
        + ", poseResult2="
        + m_poseResult2
        + "]";
  }

  private static Matrix<N3, N3> orthogonalizeRotationMatrix(Matrix<N3, N3> input) {
    var a = DecompositionFactory_DDRM.qr(3, 3);
    if (!a.decompose(input.getStorage().getDDRM())) {
      // best we can do is return the input
      return input;
    }

    // Grab results (thanks for this _great_ api, EJML)
    var Q = new DMatrixRMaj(3, 3);
    var R = new DMatrixRMaj(3, 3);
    a.getQ(Q, false);
    a.getR(R, false);

    // Fix signs in R if they're < 0 so it's close to an identity matrix
    // (our QR decomposition implementation sometimes flips the signs of columns)
    for (int colR = 0; colR < 3; ++colR) {
      if (R.get(colR, colR) < 0) {
        for (int rowQ = 0; rowQ < 3; ++rowQ) {
          Q.set(rowQ, colR, -Q.get(rowQ, colR));
        }
      }
    }

    return new Matrix<>(new SimpleMatrix(Q));
  }
}
