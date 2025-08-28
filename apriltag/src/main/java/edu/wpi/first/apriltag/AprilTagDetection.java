// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

import java.util.Arrays;
import org.wpilib.math.MatBuilder;
import org.wpilib.math.Matrix;
import org.wpilib.math.Nat;
import org.wpilib.math.numbers.N3;

/** A detection of an AprilTag tag. */
public class AprilTagDetection {
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
   * @return Homography matrix data
   */
  @SuppressWarnings("PMD.MethodReturnsInternalArray")
  public double[] getHomography() {
    return m_homography;
  }

  /**
   * Gets the 3x3 homography matrix describing the projection from an "ideal" tag (with corners at
   * (-1,1), (1,1), (1,-1), and (-1, -1)) to pixels in the image.
   *
   * @return Homography matrix
   */
  public Matrix<N3, N3> getHomographyMatrix() {
    return MatBuilder.fill(Nat.N3(), Nat.N3(), m_homography);
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
   * around the tag. Index 0 is the bottom left corner.
   *
   * @param ndx Corner index (range is 0-3, inclusive)
   * @return Corner point X coordinate
   */
  public double getCornerX(int ndx) {
    return m_corners[ndx * 2];
  }

  /**
   * Gets a corner of the tag in image pixel coordinates. These always wrap counter-clock wise
   * around the tag. Index 0 is the bottom left corner.
   *
   * @param ndx Corner index (range is 0-3, inclusive)
   * @return Corner point Y coordinate
   */
  public double getCornerY(int ndx) {
    return m_corners[ndx * 2 + 1];
  }

  /**
   * Gets the corners of the tag in image pixel coordinates. These always wrap counter-clock wise
   * around the tag. The first set of corner coordinates are the coordinates for the bottom left
   * corner.
   *
   * @return Corner point array (X and Y for each corner in order)
   */
  @SuppressWarnings("PMD.MethodReturnsInternalArray")
  public double[] getCorners() {
    return m_corners;
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
