// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sysid/analysis/FeedbackAnalysis.h"

#include <cmath>

#include <frc/controller/LinearQuadraticRegulator.h>
#include <frc/system/LinearSystem.h>
#include <units/acceleration.h>
#include <units/velocity.h>
#include <units/voltage.h>

#include "sysid/analysis/FeedbackControllerPreset.h"

using namespace sysid;

using Kv_t = decltype(1_V / 1_mps);
using Ka_t = decltype(1_V / 1_mps_sq);

FeedbackGains sysid::CalculatePositionFeedbackGains(
    const FeedbackControllerPreset& preset, const LQRParameters& params,
    double Kv, double Ka) {
  if (!std::isfinite(Kv) || !std::isfinite(Ka)) {
    return {0.0, 0.0};
  }

  // If acceleration requires no effort, velocity becomes an input for position
  // control. We choose an appropriate model in this case to avoid numerical
  // instabilities in the LQR.
  if (Ka > 1E-7) {
    // Create a position system from our feedforward gains.
    frc::LinearSystem<2, 1, 1> system{
        frc::Matrixd<2, 2>{{0.0, 1.0}, {0.0, -Kv / Ka}},
        frc::Matrixd<2, 1>{0.0, 1.0 / Ka}, frc::Matrixd<1, 2>{1.0, 0.0},
        frc::Matrixd<1, 1>{0.0}};
    // Create an LQR with 2 states to control -- position and velocity.
    frc::LinearQuadraticRegulator<2, 1> controller{
        system, {params.qp, params.qv}, {params.r}, preset.period};
    // Compensate for any latency from sensor measurements, filtering, etc.
    controller.LatencyCompensate(system, preset.period,
                                 preset.measurementDelay);

    return {
        controller.K(0, 0) * preset.outputConversionFactor,
        controller.K(0, 1) * preset.outputConversionFactor /
            (preset.normalized ? 1 : units::second_t{preset.period}.value())};
  }

  // This is our special model to avoid instabilities in the LQR.
  auto system = frc::LinearSystem<1, 1, 1>(
      Eigen::Matrix<double, 1, 1>{0.0}, Eigen::Matrix<double, 1, 1>{1.0},
      Eigen::Matrix<double, 1, 1>{1.0}, Eigen::Matrix<double, 1, 1>{0.0});
  // Create an LQR with one state -- position.
  frc::LinearQuadraticRegulator<1, 1> controller{
      system, {params.qp}, {params.r}, preset.period};
  // Compensate for any latency from sensor measurements, filtering, etc.
  controller.LatencyCompensate(system, preset.period, preset.measurementDelay);

  return {Kv * controller.K(0, 0) * preset.outputConversionFactor, 0.0};
}

FeedbackGains sysid::CalculateVelocityFeedbackGains(
    const FeedbackControllerPreset& preset, const LQRParameters& params,
    double Kv, double Ka, double encFactor) {
  if (!std::isfinite(Kv) || !std::isfinite(Ka)) {
    return {0.0, 0.0};
  }

  // If acceleration for velocity control requires no effort, the feedback
  // control gains approach zero. We special-case it here because numerical
  // instabilities arise in LQR otherwise.
  if (Ka < 1E-7) {
    return {0.0, 0.0};
  }

  // Create a velocity system from our feedforward gains.
  frc::LinearSystem<1, 1, 1> system{
      frc::Matrixd<1, 1>{-Kv / Ka}, frc::Matrixd<1, 1>{1.0 / Ka},
      frc::Matrixd<1, 1>{1.0}, frc::Matrixd<1, 1>{0.0}};
  // Create an LQR controller with 1 state -- velocity.
  frc::LinearQuadraticRegulator<1, 1> controller{
      system, {params.qv}, {params.r}, preset.period};
  // Compensate for any latency from sensor measurements, filtering, etc.
  controller.LatencyCompensate(system, preset.period, preset.measurementDelay);

  return {controller.K(0, 0) * preset.outputConversionFactor /
              (preset.outputVelocityTimeFactor * encFactor),
          0.0};
}
