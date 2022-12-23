// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

import edu.wpi.first.apriltag.jni.AprilTagJNI;
import edu.wpi.first.math.geometry.Transform3d;

/** Pose estimators for AprilTag tags. */
public final class AprilTagPoseEstimator {
  private AprilTagPoseEstimator() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

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

    public double tagSize;
    public double fx;
    public double fy;
    public double cx;
    public double cy;
  }

  /**
   * Estimates the pose of the tag using the homography method described in [1].
   *
   * @param detection Tag detection
   * @param config Estimator configuration
   * @return Pose estimate
   */
  public static Transform3d estimateHomography(AprilTagDetection detection, Config config) {
    return estimateHomography(detection.getHomography(), config);
  }

  /**
   * Estimates the pose of the tag using the homography method described in [1].
   *
   * @param homography Homography 3x3 matrix data
   * @param config Estimator configuration
   * @return Pose estimate
   */
  public static Transform3d estimateHomography(double[] homography, Config config) {
    return AprilTagJNI.estimatePoseHomography(
        homography, config.tagSize, config.fx, config.fy, config.cx, config.cy);
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
   * @param config Estimator configuration
   * @param nIters Number of iterations
   * @return Initial and (possibly) second pose estimates
   */
  public static AprilTagPoseEstimate estimateOrthogonalIteration(
      AprilTagDetection detection, Config config, int nIters) {
    return estimateOrthogonalIteration(
        detection.getHomography(), detection.getCorners(), config, nIters);
  }

  /**
   * Estimates the pose of the tag. This returns one or two possible poses for the tag, along with
   * the object-space error of each.
   *
   * @param homography Homography 3x3 matrix data
   * @param corners Corner point array (X and Y for each corner in order)
   * @param config Estimator configuration
   * @param nIters Number of iterations
   * @return Initial and (possibly) second pose estimates
   */
  public static AprilTagPoseEstimate estimateOrthogonalIteration(
      double[] homography, double[] corners, Config config, int nIters) {
    return AprilTagJNI.estimatePoseOrthogonalIteration(
        homography, corners, config.tagSize, config.fx, config.fy, config.cx, config.cy, nIters);
  }

  /**
   * Estimates tag pose. This method is an easier to use interface to
   * EstimatePoseOrthogonalIteration(), running 50 iterations and returning the pose with the lower
   * object-space error.
   *
   * @param detection Tag detection
   * @param config Estimator configuration
   * @return Pose estimate
   */
  public static Transform3d estimate(AprilTagDetection detection, Config config) {
    return estimate(detection.getHomography(), detection.getCorners(), config);
  }

  /**
   * Estimates tag pose. This method is an easier to use interface to
   * EstimatePoseOrthogonalIteration(), running 50 iterations and returning the pose with the lower
   * object-space error.
   *
   * @param homography Homography 3x3 matrix data
   * @param corners Corner point array (X and Y for each corner in order)
   * @param config Estimator configuration
   * @return Pose estimate
   */
  public static Transform3d estimate(double[] homography, double[] corners, Config config) {
    return AprilTagJNI.estimatePose(
        homography, corners, config.tagSize, config.fx, config.fy, config.cx, config.cy);
  }
}
