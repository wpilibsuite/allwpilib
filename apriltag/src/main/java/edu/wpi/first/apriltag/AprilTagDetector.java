// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

import edu.wpi.first.apriltag.jni.AprilTagJNI;
import org.opencv.core.Mat;

/**
 * An AprilTag detector engine. This is expensive to set up and tear down, so most use cases should
 * only create one of these, add a family to it, set up any other configuration, and repeatedly call
 * Detect().
 */
public class AprilTagDetector implements AutoCloseable {
  public AprilTagDetector() {
    m_native = AprilTagJNI.createDetector();
  }

  @Override
  public void close() {
    if (m_native != 0) {
      AprilTagJNI.destroyDetector(m_native);
    }
    m_native = 0;
  }

  /**
   * Sets how many threads should be used for computation. Default is single-threaded operation (1
   * thread).
   *
   * @param val number of threads
   */
  public void setNumThreads(int val) {
    AprilTagJNI.setNumThreads(m_native, val);
  }

  /**
   * Gets how many threads are being used for computation.
   *
   * @return number of threads
   */
  public int getNumThreads() {
    return AprilTagJNI.getNumThreads(m_native);
  }

  /**
   * Sets the quad decimation. Detection of quads can be done on a lower-resolution image, improving
   * speed at a cost of pose accuracy and a slight decrease in detection rate. Decoding the binary
   * payload is still done at full resolution. Default is 2.0.
   *
   * @param val decimation amount
   */
  public void setQuadDecimate(float val) {
    AprilTagJNI.setQuadDecimate(m_native, val);
  }

  /**
   * Gets the quad decimation setting.
   *
   * @return decimation amount
   */
  public float getQuadDecimate() {
    return AprilTagJNI.getQuadDecimate(m_native);
  }

  /**
   * Sets what Gaussian blur should be applied to the segmented image (used for quad detection?).
   * Very noisy images benefit from non-zero values (e.g. 0.8). Default is 0.0.
   *
   * @param val standard deviation in pixels
   */
  public void setQuadSigma(float val) {
    AprilTagJNI.setQuadSigma(m_native, val);
  }

  /**
   * Gets the Guassian blur setting.
   *
   * @return standard deviation in pixels
   */
  public float getQuadSigma() {
    return AprilTagJNI.getQuadSigma(m_native);
  }

  /**
   * Sets the refine edges option. When true, the edges of the each quad are adjusted to "snap to"
   * strong gradients nearby. This is useful when decimation is employed, as it can increase the
   * quality of the initial quad estimate substantially. Generally recommended to be on (true).
   * Default is true.
   *
   * <p>Very computationally inexpensive. Option is ignored if quad_decimate = 1.
   *
   * @param val true to enable refine edges
   */
  public void setRefineEdges(boolean val) {
    AprilTagJNI.setRefineEdges(m_native, val);
  }

  /**
   * Gets the refine edges option setting.
   *
   * @return true if refine edges is enabled
   */
  public boolean getRefineEdges() {
    return AprilTagJNI.getRefineEdges(m_native);
  }

  /**
   * Sets how much sharpening should be done to decoded images. This can help decode small tags but
   * may or may not help in odd lighting conditions or low light conditions. Default is 0.25.
   *
   * @param val sharpening amount
   */
  public void setDecodeSharpening(double val) {
    AprilTagJNI.setDecodeSharpening(m_native, val);
  }

  /**
   * Gets the decode sharpening setting.
   *
   * @return sharpening amount
   */
  public double getDecodeSharpening() {
    return AprilTagJNI.getDecodeSharpening(m_native);
  }

  /**
   * Sets debug mode. When true, the decoder writes a variety of debugging images to the current
   * working directory at various stages through the detection process. This is slow and should
   * *not* be used on space-limited systems such as the RoboRIO. Default is disabled (false).
   *
   * @param val true to enable debug mode
   */
  public void setDebug(boolean val) {
    AprilTagJNI.setDebug(m_native, val);
  }

  /**
   * Gets debug mode setting.
   *
   * @return true if debug mode is enabled
   */
  public boolean getDebug() {
    return AprilTagJNI.getDebug(m_native);
  }

  /**
   * Sets threshold used to reject quads containing too few pixels. Default is 5 pixels.
   *
   * @param val minimum number of pixels
   */
  public void setQuadMinClusterPixels(int val) {
    AprilTagJNI.setQuadMinClusterPixels(m_native, val);
  }

  /**
   * Gets minimum number of pixel threshold setting.
   *
   * @return number of pixels
   */
  public int getQuadMinClusterPixels() {
    return AprilTagJNI.getQuadMinClusterPixels(m_native);
  }

  /**
   * Sets how many corner candidates to consider when segmenting a group of pixels into a quad.
   * Default is 10.
   *
   * @param val number of candidates
   */
  public void setQuadMaxNumMaxima(int val) {
    AprilTagJNI.setQuadMaxNumMaxima(m_native, val);
  }

  /**
   * Gets corner candidates setting.
   *
   * @return number of candidates
   */
  public int getQuadMaxNumMaxima() {
    return AprilTagJNI.getQuadMaxNumMaxima(m_native);
  }

  /**
   * Sets critical angle. The detector will reject quads where pairs of edges have angles that are
   * close to straight or close to 180 degrees. Zero means that no quads are rejected. Default is 10
   * degrees.
   *
   * @param val critical angle, in radians
   */
  public void setQuadCriticalAngle(float val) {
    AprilTagJNI.setQuadCriticalAngle(m_native, val);
  }

  /**
   * Gets critical angle setting.
   *
   * @return critical angle, in radians
   */
  public float getQuadCriticalAngle() {
    return AprilTagJNI.getQuadCriticalAngle(m_native);
  }

  /**
   * When fitting lines to the contours, sets the maximum mean squared error allowed. This is useful
   * in rejecting contours that are far from being quad shaped; rejecting these quads "early" saves
   * expensive decoding processing. Default is 10.0.
   *
   * @param val allowable maximum mean squared error
   */
  public void setQuadMaxLineFitMSE(float val) {
    AprilTagJNI.setQuadMaxLineFitMSE(m_native, val);
  }

  /**
   * Gets the maximum mean squared error setting.
   *
   * @return allowable maximum mean squared error
   */
  public float getQuadMaxLineFitMSE() {
    return AprilTagJNI.getQuadMaxLineFitMSE(m_native);
  }

  /**
   * Sets minimum brightness offset. When we build our model of black &amp; white pixels, we add an
   * extra check that the white model must be (overall) brighter than the black model. How much
   * brighter? (in pixel values, [0,255]). Default is 5.
   *
   * @param val minimum offset in pixel values [0,255]
   */
  public void setQuadMinWhiteBlackDiff(int val) {
    AprilTagJNI.setQuadMinWhiteBlackDiff(m_native, val);
  }

  /**
   * Gets minimum brightness offset setting.
   *
   * @return minimum offset in pixel values [0, 255]
   */
  public int getQuadMinWhiteBlackDiff() {
    return AprilTagJNI.getQuadMinWhiteBlackDiff(m_native);
  }

  /**
   * Sets if the thresholded image be should be deglitched. Only useful for very noisy images.
   * Default is disabled (false).
   *
   * @param val true to enable
   */
  public void setQuadDeglitch(boolean val) {
    AprilTagJNI.setQuadDeglitch(m_native, val);
  }

  /**
   * Gets the deglitch setting.
   *
   * @return true if enabled
   */
  public boolean getQuadDeglitch() {
    return AprilTagJNI.getQuadDeglitch(m_native);
  }

  /**
   * Adds a family of tags to be detected.
   *
   * @param fam Family name, e.g. "tag16h5"
   * @throws IllegalArgumentException if family name not recognized
   */
  public void addFamily(String fam) {
    addFamily(fam, 2);
  }

  /**
   * Adds a family of tags to be detected.
   *
   * @param fam Family name, e.g. "tag16h5"
   * @param bitsCorrected maximum number of bits to correct
   * @throws IllegalArgumentException if family name not recognized
   */
  public void addFamily(String fam, int bitsCorrected) {
    if (!AprilTagJNI.addFamily(m_native, fam, bitsCorrected)) {
      throw new IllegalArgumentException("unknown family name '" + fam + "'");
    }
  }

  /**
   * Removes a family of tags from the detector.
   *
   * @param fam Family name, e.g. "tag16h5"
   */
  public void removeFamily(String fam) {
    AprilTagJNI.removeFamily(m_native, fam);
  }

  /** Unregister all families. */
  public void clearFamilies() {
    AprilTagJNI.clearFamilies(m_native);
  }

  /**
   * Detect tags from an 8-bit image.
   *
   * @param img 8-bit OpenCV Mat image
   * @return Results (array of AprilTagDetection)
   */
  public AprilTagDetection[] detect(Mat img) {
    return AprilTagJNI.detect(m_native, img.cols(), img.rows(), img.cols(), img.dataAddr());
  }

  private long m_native;
}
