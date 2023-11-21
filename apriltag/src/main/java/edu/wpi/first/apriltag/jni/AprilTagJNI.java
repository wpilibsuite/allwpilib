// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag.jni;

import edu.wpi.first.apriltag.AprilTagDetection;
import edu.wpi.first.apriltag.AprilTagDetector;
import edu.wpi.first.apriltag.AprilTagPoseEstimate;
import edu.wpi.first.math.geometry.Transform3d;
import edu.wpi.first.util.RuntimeLoader;
import java.io.IOException;

public class AprilTagJNI {
  static boolean libraryLoaded = false;
  static RuntimeLoader<AprilTagJNI> loader = null;

  /**
   * Load the library.
   *
   * @throws IOException if the library load failed
   */
  public static synchronized void load() throws IOException {
    if (libraryLoaded) {
      return;
    }
    loader =
        new RuntimeLoader<>(
            "apriltagjni", RuntimeLoader.getDefaultExtractionRoot(), AprilTagJNI.class);
    loader.loadLibrary();
    libraryLoaded = true;
  }

  /**
   * Constructs an AprilTag detector engine.
   *
   * @return The detector engine handle
   */
  public static native long createDetector();

  /**
   * Destroys an AprilTag detector engine.
   *
   * @param det The detector engine handle
   */
  public static native void destroyDetector(long det);

  /**
   * Sets the detector engine configuration.
   *
   * @param det The detector engine handle
   * @param config A configuration
   */
  public static native void setDetectorConfig(long det, AprilTagDetector.Config config);

  /**
   * Gets the detector engine configuration.
   *
   * @param det The detector engine handle
   * @return The configuration
   */
  public static native AprilTagDetector.Config getDetectorConfig(long det);

  /**
   * Sets the detector engine quad threshold parameters.
   *
   * @param det The detector engine handle
   * @param params Quad threshold parameters
   */
  public static native void setDetectorQTP(
      long det, AprilTagDetector.QuadThresholdParameters params);

  /**
   * Gets the detector engine quad threshold parameters.
   *
   * @param det The detector engine handle
   * @return Quad threshold parameters
   */
  public static native AprilTagDetector.QuadThresholdParameters getDetectorQTP(long det);

  /**
   * Adds a family of tags to be detected by the detector engine.
   *
   * @param det The detector engine handle
   * @param fam Family name, e.g. "tag16h5"
   * @param bitsCorrected Maximum number of bits to correct
   * @return False if family can't be found
   */
  public static native boolean addFamily(long det, String fam, int bitsCorrected);

  /**
   * Removes a family of tags from the detector.
   *
   * @param det The detector engine handle
   * @param fam Family name, e.g. "tag16h5"
   */
  public static native void removeFamily(long det, String fam);

  /**
   * Unregister all families.
   *
   * @param det The detector engine handle
   */
  public static native void clearFamilies(long det);

  /**
   * Detect tags from an 8-bit image.
   *
   * @param det The detector engine handle
   * @param width The width of the image
   * @param height The height of the image
   * @param stride The number of bytes between image rows (often the same as width)
   * @param bufAddr The address of the image buffer
   * @return The results (array of AprilTagDetection)
   */
  public static native AprilTagDetection[] detect(
      long det, int width, int height, int stride, long bufAddr);

  /**
   * Estimates the pose of the tag using the homography method described in [1].
   *
   * @param homography Homography 3x3 matrix data
   * @param tagSize The tag size, in meters
   * @param fx The camera horizontal focal length, in pixels
   * @param fy The camera vertical focal length, in pixels
   * @param cx The camera horizontal focal center, in pixels
   * @param cy The camera vertical focal center, in pixels
   * @return Pose estimate
   */
  public static native Transform3d estimatePoseHomography(
      double[] homography, double tagSize, double fx, double fy, double cx, double cy);

  /**
   * Estimates the pose of the tag. This returns one or two possible poses for the tag, along with
   * the object-space error of each.
   *
   * @param homography Homography 3x3 matrix data
   * @param corners Corner point array (X and Y for each corner in order)
   * @param tagSize The tag size, in meters
   * @param fx The camera horizontal focal length, in pixels
   * @param fy The camera vertical focal length, in pixels
   * @param cx The camera horizontal focal center, in pixels
   * @param cy The camera vertical focal center, in pixels
   * @param nIters Number of iterations
   * @return Initial and (possibly) second pose estimates
   */
  public static native AprilTagPoseEstimate estimatePoseOrthogonalIteration(
      double[] homography,
      double[] corners,
      double tagSize,
      double fx,
      double fy,
      double cx,
      double cy,
      int nIters);

  /**
   * Estimates tag pose. This method is an easier to use interface to
   * EstimatePoseOrthogonalIteration(), running 50 iterations and returning the pose with the lower
   * object-space error.
   *
   * @param homography Homography 3x3 matrix data
   * @param corners Corner point array (X and Y for each corner in order)
   * @param tagSize The tag size, in meters
   * @param fx The camera horizontal focal length, in pixels
   * @param fy The camera vertical focal length, in pixels
   * @param cx The camera horizontal focal center, in pixels
   * @param cy The camera vertical focal center, in pixels
   * @return Pose estimate
   */
  public static native Transform3d estimatePose(
      double[] homography,
      double[] corners,
      double tagSize,
      double fx,
      double fy,
      double cx,
      double cy);
}
