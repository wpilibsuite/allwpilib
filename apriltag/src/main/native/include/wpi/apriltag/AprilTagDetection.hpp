// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <string_view>

#include <Eigen/Core>
#include <wpi/SymbolExports.h>

namespace frc {

/**
 * A detection of an AprilTag tag.
 */
class WPILIB_DLLEXPORT AprilTagDetection final {
 public:
  AprilTagDetection() = delete;
  AprilTagDetection(const AprilTagDetection&) = delete;
  AprilTagDetection& operator=(const AprilTagDetection&) = delete;

  /** A point. Used for center and corner points. */
  struct Point {
    double x;
    double y;
  };

  /**
   * Gets the decoded tag's family name.
   *
   * @return Decoded family name
   */
  std::string_view GetFamily() const;

  /**
   * Gets the decoded ID of the tag.
   *
   * @return Decoded ID
   */
  int GetId() const { return id; }

  /**
   * Gets how many error bits were corrected. Note: accepting large numbers of
   * corrected errors leads to greatly increased false positive rates.
   * NOTE: As of this implementation, the detector cannot detect tags with
   * a hamming distance greater than 2.
   *
   * @return Hamming distance (number of corrected error bits)
   */
  int GetHamming() const { return hamming; }

  /**
   * Gets a measure of the quality of the binary decoding process: the
   * average difference between the intensity of a data bit versus
   * the decision threshold. Higher numbers roughly indicate better
   * decodes. This is a reasonable measure of detection accuracy
   * only for very small tags-- not effective for larger tags (where
   * we could have sampled anywhere within a bit cell and still
   * gotten a good detection.)
   *
   * @return Decision margin
   */
  float GetDecisionMargin() const { return decision_margin; }

  /**
   * Gets the 3x3 homography matrix describing the projection from an
   * "ideal" tag (with corners at (-1,1), (1,1), (1,-1), and (-1,
   * -1)) to pixels in the image.
   *
   * @return Homography matrix data
   */
  std::span<const double, 9> GetHomography() const;

  /**
   * Gets the 3x3 homography matrix describing the projection from an
   * "ideal" tag (with corners at (-1,1), (1,1), (1,-1), and (-1,
   * -1)) to pixels in the image.
   *
   * @return Homography matrix
   */
  Eigen::Matrix3d GetHomographyMatrix() const;

  /**
   * Gets the center of the detection in image pixel coordinates.
   *
   * @return Center point
   */
  const Point& GetCenter() const { return *reinterpret_cast<const Point*>(c); }

  /**
   * Gets a corner of the tag in image pixel coordinates. These always
   * wrap counter-clock wise around the tag. Index 0 is the bottom left corner.
   *
   * @param ndx Corner index (range is 0-3, inclusive)
   * @return Corner point
   */
  const Point& GetCorner(int ndx) const {
    return *reinterpret_cast<const Point*>(p[ndx]);
  }

  /**
   * Gets the corners of the tag in image pixel coordinates. These always
   * wrap counter-clock wise around the tag. The first set of corner coordinates
   * are the coordinates for the bottom left corner.
   *
   * @param cornersBuf Corner point array (X and Y for each corner in order)
   * @return Corner point array (copy of cornersBuf span)
   */
  std::span<double, 8> GetCorners(std::span<double, 8> cornersBuf) const {
    for (int i = 0; i < 4; i++) {
      cornersBuf[i * 2] = p[i][0];
      cornersBuf[i * 2 + 1] = p[i][1];
    }
    return cornersBuf;
  }

 private:
  // This class *must* be standard-layout-compatible with apriltag_detection
  // as we use reinterpret_cast from that structure. This means the below
  // members must exactly match the contents of the apriltag_detection struct.

  // The tag family.
  void* family;

  // The decoded ID of the tag.
  int id;

  // How many error bits were corrected? Note: accepting large numbers of
  // corrected errors leads to greatly increased false positive rates.
  // NOTE: As of this implementation, the detector cannot detect tags with
  // a hamming distance greater than 2.
  int hamming;

  // A measure of the quality of the binary decoding process: the
  // average difference between the intensity of a data bit versus
  // the decision threshold. Higher numbers roughly indicate better
  // decodes. This is a reasonable measure of detection accuracy
  // only for very small tags-- not effective for larger tags (where
  // we could have sampled anywhere within a bit cell and still
  // gotten a good detection.)
  float decision_margin;

  // The 3x3 homography matrix describing the projection from an
  // "ideal" tag (with corners at (-1,1), (1,1), (1,-1), and (-1,
  // -1)) to pixels in the image.
  void* H;

  // The center of the detection in image pixel coordinates.
  double c[2];

  // The corners of the tag in image pixel coordinates. These always
  // wrap counter-clock wise around the tag.
  double p[4][2];
};

}  // namespace frc
