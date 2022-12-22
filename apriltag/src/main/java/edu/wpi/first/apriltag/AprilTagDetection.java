// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

import edu.wpi.first.apriltag.jni.AprilTagJNI;
import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.geometry.Transform3d;
import edu.wpi.first.math.numbers.N3;
import java.util.Arrays;

/** A detection of an AprilTag tag. */
public class AprilTagDetection {
  /** A pose estimate. */
  public class PoseEstimate {
    public PoseEstimate(Transform3d pose1, Transform3d pose2, double error1, double error2) {
      this.pose1 = pose1;
      this.pose2 = pose2;
      this.error1 = error1;
      this.error2 = error2;
    }

    /**
     * Get the ratio of pose reprojection errors, called ambiguity. Numbers above 0.2 are likely to
     * be ambiguous.
     *
     * @return The ratio of pose reprojection errors.
     */
    public double getAmbiguity() {
      double min = Math.min(error1, error2);
      double max = Math.max(error1, error2);

      if (max > 0) {
        return min / max;
      } else {
        return -1;
      }
    }

    /** Pose 1. */
    public final Transform3d pose1;

    /** Pose 2. */
    public final Transform3d pose2;

    /** Object-space error of pose 1. */
    public final double error1;

    /** Object-space error of pose 2. */
    public final double error2;
  }

  /** Configuration for the pose estimator. */
  public class PoseEstimatorConfig {
    public double tagSize; // in meters
    public double fx; // in pixels
    public double fy; // in pixels
    public double cx; // in pixels
    public double cy; // in pixels
  }

  /**
   * Gets the decoded tag's family name.
   *
   * @return Decoded family name
   */
  public String getFamily() {
    return m_family;
  }

  /**
   * Gets the decoded ID of the tag.
   *
   * @return Decoded ID
   */
  public int getId() {
    return m_id;
  }

  /**
   * Gets how many error bits were corrected. Note: accepting large numbers of corrected errors
   * leads to greatly increased false positive rates. NOTE: As of this implementation, the detector
   * cannot detect tags with a hamming distance greater than 2.
   *
   * @return Hamming distance (number of corrected error bits)
   */
  public int getHamming() {
    return m_hamming;
  }

  /**
   * Gets a measure of the quality of the binary decoding process: the average difference between
   * the intensity of a data bit versus the decision threshold. Higher numbers roughly indicate
   * better decodes. This is a reasonable measure of detection accuracy only for very small tags--
   * not effective for larger tags (where we could have sampled anywhere within a bit cell and still
   * gotten a good detection.)
   *
   * @return Decision margin
   */
  public float getDecisionMargin() {
    return m_decisionMargin;
  }

  /**
   * Gets the 3x3 homography matrix describing the projection from an "ideal" tag (with corners at
   * (-1,1), (1,1), (1,-1), and (-1, -1)) to pixels in the image.
   *
   * @return Homography matrix
   */
  public Matrix<N3, N3> getHomographyMatrix() {
    return new MatBuilder<>(Nat.N3(), Nat.N3()).fill(m_homography);
  }

  /**
   * Gets the center of the detection in image pixel coordinates.
   *
   * @return Center point X coordinate
   */
  public double getCenterX() {
    return m_centerX;
  }

  /**
   * Gets the center of the detection in image pixel coordinates.
   *
   * @return Center point Y coordinate
   */
  public double getCenterY() {
    return m_centerY;
  }

  /**
   * Gets a corner of the tag in image pixel coordinates. These always wrap counter-clock wise
   * around the tag.
   *
   * @param ndx Corner index (range is 0-3, inclusive)
   * @return Corner point X coordinate
   */
  public double GetCornerX(int ndx) {
    return m_corners[ndx * 2];
  }

  /**
   * Gets a corner of the tag in image pixel coordinates. These always wrap counter-clock wise
   * around the tag.
   *
   * @param ndx Corner index (range is 0-3, inclusive)
   * @return Corner point Y coordinate
   */
  public double GetCornerY(int ndx) {
    return m_corners[ndx * 2 + 1];
  }

  /**
   * Gets the corners of the tag in image pixel coordinates. These always wrap counter-clock wise
   * around the tag.
   *
   * @return Corner point array (X and Y for each corner in order)
   */
  @SuppressWarnings("PMD.MethodReturnsInternalArray")
  public double[] getCorners() {
    return m_corners;
  }

  /**
   * Estimates the pose of the tag using the homography method described in [1].
   *
   * @param config Estimator configuration
   * @return Pose estimate
   */
  public Transform3d estimatePoseHomography(PoseEstimatorConfig config) {
    return AprilTagJNI.estimatePoseHomography(
        m_homography, config.tagSize, config.fx, config.fy, config.cx, config.cy);
  }

  /**
   * Estimates the pose of the tag. This returns one or two possible poses for the tag, along with
   * the object-space error of each.
   *
   * <p>This uses the homography method described in [1] for the initial estimate. Then Orthogonal
   * Iteration [2] is used to refine this estimate. Then [3] is used to find a potential second
   * local minima and Orthogonal Iteration is used to refine this second estimate.
   *
   * <p>[1]: E. Olson, “Apriltag: A robust and flexible visual fiducial system,” in 2011 IEEE
   * International Conference on Robotics and Automation, May 2011, pp. 3400–3407.
   *
   * <p>[2]: Lu, G. D. Hager and E. Mjolsness, "Fast and globally convergent pose estimation from
   * video images," in IEEE Transactions on Pattern Analysis and Machine Intelligence, vol. 22, no.
   * 6, pp. 610-622, June 2000. doi: 10.1109/34.862199
   *
   * <p>[3]: Schweighofer and A. Pinz, "Robust Pose Estimation from a Planar Target," in IEEE
   * Transactions on Pattern Analysis and Machine Intelligence, vol. 28, no. 12, pp. 2024-2030, Dec.
   * 2006. doi: 10.1109/TPAMI.2006.252
   *
   * @param config Estimator configuration
   * @param nIters Number of iterations
   * @return Initial and (possibly) second pose estimates
   */
  public PoseEstimate estimatePoseOrthogonalIteration(PoseEstimatorConfig config, int nIters) {
    return AprilTagJNI.estimatePoseOrthogonalIteration(
        m_homography,
        m_corners,
        config.tagSize,
        config.fx,
        config.fy,
        config.cx,
        config.cy,
        nIters);
  }

  /**
   * Estimates tag pose. This method is an easier to use interface to
   * EstimatePoseOrthogonalIteration(), running 50 iterations and returning the pose with the lower
   * object-space error.
   *
   * @param config Estimator configuration
   * @return Pose estimate
   */
  public Transform3d estimatePose(PoseEstimatorConfig config) {
    return AprilTagJNI.estimatePose(
        m_homography, m_corners, config.tagSize, config.fx, config.fy, config.cx, config.cy);
  }

  private final String m_family;
  private final int m_id;
  private final int m_hamming;
  private final float m_decisionMargin;
  private final double[] m_homography;
  private final double m_centerX;
  private final double m_centerY;
  private final double[] m_corners;

  /**
   * Constructs a new detection result. Used from JNI.
   *
   * @param family family
   * @param id id
   * @param hamming hamming
   * @param decisionMargin dm
   * @param homography homography
   * @param centerX centerX
   * @param centerY centerY
   * @param corners corners
   */
  @SuppressWarnings("PMD.ArrayIsStoredDirectly")
  public AprilTagDetection(
      String family,
      int id,
      int hamming,
      float decisionMargin,
      double[] homography,
      double centerX,
      double centerY,
      double[] corners) {
    m_family = family;
    m_id = id;
    m_hamming = hamming;
    m_decisionMargin = decisionMargin;
    m_homography = homography;
    m_centerX = centerX;
    m_centerY = centerY;
    m_corners = corners;
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
        + ", hamming="
        + m_hamming
        + ", homography="
        + Arrays.toString(m_homography)
        + ", family="
        + m_family
        + ", id="
        + m_id
        + "]";
  }
}
