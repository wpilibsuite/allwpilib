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
  /** Detector configuration. */
  @SuppressWarnings("MemberName")
  public static class Config {
    /**
     * How many threads should be used for computation. Default is single-threaded operation (1
     * thread).
     */
    public int numThreads = 1;

    /**
     * Quad decimation. Detection of quads can be done on a lower-resolution image, improving speed
     * at a cost of pose accuracy and a slight decrease in detection rate. Decoding the binary
     * payload is still done at full resolution. Default is 2.0.
     */
    public float quadDecimate = 2.0f;

    /**
     * What Gaussian blur should be applied to the segmented image (used for quad detection). Very
     * noisy images benefit from non-zero values (e.g. 0.8). Default is 0.0.
     */
    public float quadSigma;

    /**
     * When true, the edges of the each quad are adjusted to "snap to" strong gradients nearby. This
     * is useful when decimation is employed, as it can increase the quality of the initial quad
     * estimate substantially. Generally recommended to be on (true). Default is true.
     *
     * <p>Very computationally inexpensive. Option is ignored if quad_decimate = 1.
     */
    public boolean refineEdges = true;

    /**
     * How much sharpening should be done to decoded images. This can help decode small tags but may
     * or may not help in odd lighting conditions or low light conditions. Default is 0.25.
     */
    public double decodeSharpening = 0.25;

    /**
     * Debug mode. When true, the decoder writes a variety of debugging images to the current
     * working directory at various stages through the detection process. This is slow and should
     * *not* be used on space-limited systems such as the RoboRIO. Default is disabled (false).
     */
    public boolean debug;

    /** Default constructor. */
    public Config() {}

    /**
     * Constructs a detector configuration.
     *
     * @param numThreads How many threads should be used for computation.
     * @param quadDecimate Quad decimation.
     * @param quadSigma What Gaussian blur should be applied to the segmented image (used for quad
     *     detection).
     * @param refineEdges When true, the edges of the each quad are adjusted to "snap to" strong
     *     gradients nearby.
     * @param decodeSharpening How much sharpening should be done to decoded images.
     * @param debug Debug mode.
     */
    Config(
        int numThreads,
        float quadDecimate,
        float quadSigma,
        boolean refineEdges,
        double decodeSharpening,
        boolean debug) {
      this.numThreads = numThreads;
      this.quadDecimate = quadDecimate;
      this.quadSigma = quadSigma;
      this.refineEdges = refineEdges;
      this.decodeSharpening = decodeSharpening;
      this.debug = debug;
    }

    @Override
    public int hashCode() {
      return numThreads
          + Float.hashCode(quadDecimate)
          + Float.hashCode(quadSigma)
          + Boolean.hashCode(refineEdges)
          + Double.hashCode(decodeSharpening)
          + Boolean.hashCode(debug);
    }

    @Override
    public boolean equals(Object obj) {
      return obj instanceof Config other
          && numThreads == other.numThreads
          && quadDecimate == other.quadDecimate
          && quadSigma == other.quadSigma
          && refineEdges == other.refineEdges
          && decodeSharpening == other.decodeSharpening
          && debug == other.debug;
    }
  }

  /** Quad threshold parameters. */
  @SuppressWarnings("MemberName")
  public static class QuadThresholdParameters {
    /** Threshold used to reject quads containing too few pixels. Default is 300 pixels. */
    public int minClusterPixels = 300;

    /**
     * How many corner candidates to consider when segmenting a group of pixels into a quad. Default
     * is 10.
     */
    public int maxNumMaxima = 10;

    /**
     * Critical angle, in radians. The detector will reject quads where pairs of edges have angles
     * that are close to straight or close to 180 degrees. Zero means that no quads are rejected.
     * Default is 45 degrees.
     */
    public double criticalAngle = 45 * Math.PI / 180.0;

    /**
     * When fitting lines to the contours, the maximum mean squared error allowed. This is useful in
     * rejecting contours that are far from being quad shaped; rejecting these quads "early" saves
     * expensive decoding processing. Default is 10.0.
     */
    public float maxLineFitMSE = 10.0f;

    /**
     * Minimum brightness offset. When we build our model of black &amp; white pixels, we add an
     * extra check that the white model must be (overall) brighter than the black model. How much
     * brighter? (in pixel values, [0,255]). Default is 5.
     */
    public int minWhiteBlackDiff = 5;

    /**
     * Whether the thresholded image be should be deglitched. Only useful for very noisy images.
     * Default is disabled (false).
     */
    public boolean deglitch;

    /** Default constructor. */
    public QuadThresholdParameters() {}

    /**
     * Constructs quad threshold parameters.
     *
     * @param minClusterPixels Threshold used to reject quads containing too few pixels.
     * @param maxNumMaxima How many corner candidates to consider when segmenting a group of pixels
     *     into a quad.
     * @param criticalAngle Critical angle, in radians.
     * @param maxLineFitMSE When fitting lines to the contours, the maximum mean squared error
     *     allowed.
     * @param minWhiteBlackDiff Minimum brightness offset.
     * @param deglitch Whether the thresholded image be should be deglitched.
     */
    QuadThresholdParameters(
        int minClusterPixels,
        int maxNumMaxima,
        double criticalAngle,
        float maxLineFitMSE,
        int minWhiteBlackDiff,
        boolean deglitch) {
      this.minClusterPixels = minClusterPixels;
      this.maxNumMaxima = maxNumMaxima;
      this.criticalAngle = criticalAngle;
      this.maxLineFitMSE = maxLineFitMSE;
      this.minWhiteBlackDiff = minWhiteBlackDiff;
      this.deglitch = deglitch;
    }

    @Override
    public int hashCode() {
      return minClusterPixels
          + maxNumMaxima
          + Double.hashCode(criticalAngle)
          + Float.hashCode(maxLineFitMSE)
          + minWhiteBlackDiff
          + Boolean.hashCode(deglitch);
    }

    @Override
    public boolean equals(Object obj) {
      return obj instanceof QuadThresholdParameters other
          && minClusterPixels == other.minClusterPixels
          && maxNumMaxima == other.maxNumMaxima
          && criticalAngle == other.criticalAngle
          && maxLineFitMSE == other.maxLineFitMSE
          && minWhiteBlackDiff == other.minWhiteBlackDiff
          && deglitch == other.deglitch;
    }
  }

  /** Constructs an AprilTagDetector. */
  @SuppressWarnings("this-escape")
  public AprilTagDetector() {
    m_native = AprilTagJNI.createDetector();
    setQuadThresholdParameters(new QuadThresholdParameters());
  }

  @Override
  public void close() {
    if (m_native != 0) {
      AprilTagJNI.destroyDetector(m_native);
    }
    m_native = 0;
  }

  /**
   * Sets detector configuration.
   *
   * @param config Configuration
   */
  public void setConfig(Config config) {
    AprilTagJNI.setDetectorConfig(m_native, config);
  }

  /**
   * Gets detector configuration.
   *
   * @return Configuration
   */
  public Config getConfig() {
    return AprilTagJNI.getDetectorConfig(m_native);
  }

  /**
   * Sets quad threshold parameters.
   *
   * @param params Parameters
   */
  public void setQuadThresholdParameters(QuadThresholdParameters params) {
    AprilTagJNI.setDetectorQTP(m_native, params);
  }

  /**
   * Gets quad threshold parameters.
   *
   * @return Parameters
   */
  public QuadThresholdParameters getQuadThresholdParameters() {
    return AprilTagJNI.getDetectorQTP(m_native);
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
   * @param bitsCorrected Maximum number of bits to correct
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
   * <p>The image must be grayscale.
   *
   * @param img 8-bit OpenCV Mat image
   * @return Results (array of AprilTagDetection)
   */
  public AprilTagDetection[] detect(Mat img) {
    return AprilTagJNI.detect(m_native, img.cols(), img.rows(), (int) img.step1(), img.dataAddr());
  }

  private long m_native;
}
