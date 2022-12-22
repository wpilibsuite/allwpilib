// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <string_view>

#include <wpi/SymbolExports.h>

#include "frc/EigenCore.h"
#include "frc/geometry/Transform3d.h"

namespace frc {

/**
 * A detection of an AprilTag tag.
 */
class WPILIB_DLLEXPORT AprilTagDetection {
 public:
  /** A point. Used for center and corner points. */
  struct Point {
    double x;
    double y;
  };

  /** A pair of pose estimates. */
  struct PoseEstimate {
    /** Pose 1. */
    Transform3d pose1;

    /** Pose 2. */
    Transform3d pose2;

    /** Object-space error of pose 1. */
    double error1;

    /** Object-space error of pose 2. */
    double error2;

    /**
     * Gets the ratio of pose reprojection errors, called ambiguity. Numbers
     * above 0.2 are likely to be ambiguous.
     *
     * @return The ratio of pose reprojection errors.
     */
    double GetAmbiguity() const;
  };

  /** Configuration for the pose estimator. */
  struct PoseEstimatorConfig {
    double tagSize;  // in meters
    double fx;       // in pixels
    double fy;       // in pixels
    double cx;       // in pixels
    double cy;       // in pixels
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
   * wrap counter-clock wise around the tag.
   *
   * @param ndx Corner index (range is 0-3, inclusive)
   * @return Corner point
   */
  const Point& GetCorner(int ndx) const {
    return *reinterpret_cast<const Point*>(p[ndx]);
  }

  /**
   * Estimates the pose of the tag using the homography method described in [1].
   *
   * @param config Estimator configuration
   * @return Pose estimate
   */
  Transform3d EstimatePoseHomography(const PoseEstimatorConfig& config) const;

  /**
   * Estimates the pose of the tag. This returns one or two possible poses for
   * the tag, along with the object-space error of each.
   *
   * This uses the homography method described in [1] for the initial estimate.
   * Then Orthogonal Iteration [2] is used to refine this estimate. Then [3] is
   * used to find a potential second local minima and Orthogonal Iteration is
   * used to refine this second estimate.
   *
   * [1]: E. Olson, “Apriltag: A robust and flexible visual fiducial system,” in
   *      2011 IEEE International Conference on Robotics and Automation,
   *      May 2011, pp. 3400–3407.
   * [2]: Lu, G. D. Hager and E. Mjolsness, "Fast and globally convergent pose
   *      estimation from video images," in IEEE Transactions on Pattern
   * Analysis and Machine Intelligence, vol. 22, no. 6, pp. 610-622, June 2000.
   *      doi: 10.1109/34.862199
   * [3]: Schweighofer and A. Pinz, "Robust Pose Estimation from a Planar
   * Target," in IEEE Transactions on Pattern Analysis and Machine Intelligence,
   *      vol. 28, no. 12, pp. 2024-2030, Dec. 2006. doi: 10.1109/TPAMI.2006.252
   *
   * @param config Estimator configuration
   * @param nIters Number of iterations
   * @return Initial and (possibly) second pose estimates
   */
  PoseEstimate EstimatePoseOrthogonalIteration(
      const PoseEstimatorConfig& config, int nIters) const;

  /**
   * Estimates tag pose. This method is an easier to use interface to
   * EstimatePoseOrthogonalIteration(), running 50 iterations and returning the
   * pose with the lower object-space error.
   *
   * @param config Estimator configuration
   * @return Pose estimate
   */
  Transform3d EstimatePose(const PoseEstimatorConfig& config) const;

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
