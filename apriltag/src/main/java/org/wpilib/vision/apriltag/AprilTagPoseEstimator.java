// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

import edu.wpi.first.apriltag.jni.AprilTagJNI;
import edu.wpi.first.math.geometry.Transform3d;

/** Pose estimators for AprilTag tags. */
public class AprilTagPoseEstimator {
  /** Configuration for the pose estimator. */
  @SuppressWarnings("MemberName")
  public static class Config {
    /**
     * Creates a pose estimator configuration.
     *
     * @param tagSize tag size, in meters
     * @param fx camera horizontal focal length, in pixels
     * @param fy camera vertical focal length, in pixels
     * @param cx camera horizontal focal center, in pixels
     * @param cy camera vertical focal center, in pixels
     */
    public Config(double tagSize, double fx, double fy, double cx, double cy) {
      this.tagSize = tagSize;
      this.fx = fx;
      this.fy = fy;
      this.cx = cx;
      this.cy = cy;
    }

    /** Tag size, in meters. */
    public double tagSize;

    /** Camera horizontal focal length, in pixels. */
    public double fx;

    /** Camera vertical focal length, in pixels. */
    public double fy;

    /** Camera horizontal focal center, in pixels. */
    public double cx;

    /** Camera vertical focal center, in pixels. */
    public double cy;

    @Override
    public int hashCode() {
      return Double.hashCode(tagSize)
          + Double.hashCode(fx)
          + Double.hashCode(fy)
          + Double.hashCode(cx)
          + Double.hashCode(cy);
    }

    @Override
    public boolean equals(Object obj) {
      return obj instanceof Config other
          && tagSize == other.tagSize
          && fx == other.fx
          && fy == other.fy
          && cx == other.cx
          && cy == other.cy;
    }
  }

  /**
   * Creates estimator.
   *
   * @param config Configuration
   */
  public AprilTagPoseEstimator(Config config) {
    m_config = new Config(config.tagSize, config.fx, config.fy, config.cx, config.cy);
  }

  /**
   * Sets estimator configuration.
   *
   * @param config Configuration
   */
  public void setConfig(Config config) {
    m_config.tagSize = config.tagSize;
    m_config.fx = config.fx;
    m_config.fy = config.fy;
    m_config.cx = config.cx;
    m_config.cy = config.cy;
  }

  /**
   * Gets estimator configuration.
   *
   * @return Configuration
   */
  public Config getConfig() {
    return new Config(m_config.tagSize, m_config.fx, m_config.fy, m_config.cx, m_config.cy);
  }

  /**
   * Estimates the pose of the tag using the homography method described in [1].
   *
   * @param detection Tag detection
   * @return Pose estimate
   */
  public Transform3d estimateHomography(AprilTagDetection detection) {
    return estimateHomography(detection.getHomography());
  }

  /**
   * Estimates the pose of the tag using the homography method described in [1].
   *
   * @param homography Homography 3x3 matrix data
   * @return Pose estimate
   */
  public Transform3d estimateHomography(double[] homography) {
    return AprilTagJNI.estimatePoseHomography(
        homography, m_config.tagSize, m_config.fx, m_config.fy, m_config.cx, m_config.cy);
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
   * @param detection Tag detection
   * @param nIters Number of iterations
   * @return Initial and (possibly) second pose estimates
   */
  public AprilTagPoseEstimate estimateOrthogonalIteration(AprilTagDetection detection, int nIters) {
    return estimateOrthogonalIteration(detection.getHomography(), detection.getCorners(), nIters);
  }

  /**
   * Estimates the pose of the tag. This returns one or two possible poses for the tag, along with
   * the object-space error of each.
   *
   * @param homography Homography 3x3 matrix data
   * @param corners Corner point array (X and Y for each corner in order)
   * @param nIters Number of iterations
   * @return Initial and (possibly) second pose estimates
   */
  public AprilTagPoseEstimate estimateOrthogonalIteration(
      double[] homography, double[] corners, int nIters) {
    return AprilTagJNI.estimatePoseOrthogonalIteration(
        homography,
        corners,
        m_config.tagSize,
        m_config.fx,
        m_config.fy,
        m_config.cx,
        m_config.cy,
        nIters);
  }

  /**
   * Estimates tag pose. This method is an easier to use interface to
   * EstimatePoseOrthogonalIteration(), running 50 iterations and returning the pose with the lower
   * object-space error.
   *
   * @param detection Tag detection
   * @return Pose estimate
   */
  public Transform3d estimate(AprilTagDetection detection) {
    return estimate(detection.getHomography(), detection.getCorners());
  }

  /**
   * Estimates tag pose. This method is an easier to use interface to
   * EstimatePoseOrthogonalIteration(), running 50 iterations and returning the pose with the lower
   * object-space error.
   *
   * @param homography Homography 3x3 matrix data
   * @param corners Corner point array (X and Y for each corner in order)
   * @return Pose estimate
   */
  public Transform3d estimate(double[] homography, double[] corners) {
    return AprilTagJNI.estimatePose(
        homography, corners, m_config.tagSize, m_config.fx, m_config.fy, m_config.cx, m_config.cy);
  }

  private final Config m_config;
}
