// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>

#include <units/length.h>
#include <wpi/SymbolExports.h>

#include "frc/apriltag/AprilTagPoseEstimate.h"
#include "frc/geometry/Transform3d.h"

namespace frc {

class AprilTagDetection;

/** Pose estimators for AprilTag tags. */
class WPILIB_DLLEXPORT AprilTagPoseEstimator {
 public:
  /** Configuration for the pose estimator. */
  struct Config {
    bool operator==(const Config&) const = default;

    /** The tag size. */
    units::meter_t tagSize;

    /** Camera horizontal focal length, in pixels. */
    double fx;

    /** Camera vertical focal length, in pixels. */
    double fy;

    /** Camera horizontal focal center, in pixels. */
    double cx;

    /** Camera vertical focal center, in pixels. */
    double cy;
  };

  /**
   * Creates estimator.
   *
   * @param config Configuration
   */
  explicit AprilTagPoseEstimator(const Config& config) : m_config{config} {}

  /**
   * Sets estimator configuration.
   *
   * @param config Configuration
   */
  void SetConfig(const Config& config) { m_config = config; }

  /**
   * Gets estimator configuration.
   *
   * @return Configuration
   */
  const Config& GetConfig() const { return m_config; }

  /**
   * Estimates the pose of the tag using the homography method described in [1].
   *
   * @param detection Tag detection
   * @return Pose estimate
   */
  Transform3d EstimateHomography(const AprilTagDetection& detection) const;

  /**
   * Estimates the pose of the tag using the homography method described in [1].
   *
   * @param homography Homography 3x3 matrix data
   * @return Pose estimate
   */
  Transform3d EstimateHomography(std::span<const double, 9> homography) const;

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
   * @param detection Tag detection
   * @param nIters Number of iterations
   * @return Initial and (possibly) second pose estimates
   */
  AprilTagPoseEstimate EstimateOrthogonalIteration(
      const AprilTagDetection& detection, int nIters) const;

  /**
   * Estimates the pose of the tag. This returns one or two possible poses for
   * the tag, along with the object-space error of each.
   *
   * @param homography Homography 3x3 matrix data
   * @param corners Corner point array (X and Y for each corner in order)
   * @param nIters Number of iterations
   * @return Initial and (possibly) second pose estimates
   */
  AprilTagPoseEstimate EstimateOrthogonalIteration(
      std::span<const double, 9> homography, std::span<const double, 8> corners,
      int nIters) const;

  /**
   * Estimates tag pose. This method is an easier to use interface to
   * EstimatePoseOrthogonalIteration(), running 50 iterations and returning the
   * pose with the lower object-space error.
   *
   * @param detection Tag detection
   * @return Pose estimate
   */
  Transform3d Estimate(const AprilTagDetection& detection) const;

  /**
   * Estimates tag pose. This method is an easier to use interface to
   * EstimatePoseOrthogonalIteration(), running 50 iterations and returning the
   * pose with the lower object-space error.
   *
   * @param homography Homography 3x3 matrix data
   * @param corners Corner point array (X and Y for each corner in order)
   * @return Pose estimate
   */
  Transform3d Estimate(std::span<const double, 9> homography,
                       std::span<const double, 8> corners) const;

 private:
  Config m_config;
};

}  // namespace frc
