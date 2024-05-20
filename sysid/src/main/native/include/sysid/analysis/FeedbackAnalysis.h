// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace sysid {

struct FeedbackControllerPreset;

/**
 * Represents parameters used to calculate optimal feedback gains using a
 * linear-quadratic regulator (LQR).
 */
struct LQRParameters {
  /**
   * The maximum allowable deviation in position.
   */
  double qp;

  /**
   * The maximum allowable deviation in velocity.
   */
  double qv;

  /**
   * The maximum allowable control effort.
   */
  double r;
};

/**
 * Stores feedback controller gains.
 */
struct FeedbackGains {
  /**
   * The calculated Proportional gain
   */
  double Kp;

  /**
   * The calculated Derivative gain
   */
  double Kd;
};

/**
 * Calculates position feedback gains for the given controller preset, LQR
 * controller gain parameters and feedforward gains.
 *
 * @param preset The feedback controller preset.
 * @param params The parameters for calculating optimal feedback gains.
 * @param Kv     Velocity feedforward gain.
 * @param Ka     Acceleration feedforward gain.
 */
FeedbackGains CalculatePositionFeedbackGains(
    const FeedbackControllerPreset& preset, const LQRParameters& params,
    double Kv, double Ka);

/**
 * Calculates velocity feedback gains for the given controller preset, LQR
 * controller gain parameters and feedforward gains.
 *
 * @param preset           The feedback controller preset.
 * @param params           The parameters for calculating optimal feedback
 *                         gains.
 * @param Kv               Velocity feedforward gain.
 * @param Ka               Acceleration feedforward gain.
 * @param encFactor        The factor to convert the gains from output units to
 *                         encoder units. This is usually encoder EPR * gearing
 *                         * units per rotation.
 */
FeedbackGains CalculateVelocityFeedbackGains(
    const FeedbackControllerPreset& preset, const LQRParameters& params,
    double Kv, double Ka, double encFactor = 1.0);
}  // namespace sysid
