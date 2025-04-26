// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sysid/analysis/FeedbackAnalysis.h"

#include <cmath>

#include <frc/controller/LinearQuadraticRegulator.h>
#include <frc/system/LinearSystem.h>
#include <frc/system/plant/LinearSystemId.h>
#include <units/acceleration.h>
#include <units/velocity.h>
#include <units/voltage.h>

#include "sysid/analysis/FeedbackControllerPreset.h"

using namespace sysid;

using Kv_t = decltype(1_V / 1_mps);
using Ka_t = decltype(1_V / 1_mps_sq);
using Matrix1d = Eigen::Matrix<double, 1, 1>;

FeedbackGains sysid::CalculatePositionFeedbackGains(
    const FeedbackControllerPreset& preset, const LQRParameters& params,
    double Kv, double Ka) {
  if (!std::isfinite(Kv) || !std::isfinite(Ka)) {
    return {0.0, 0.0};
  }

  // If acceleration for position control requires no effort, velocity becomes
  // an input. We choose an appropriate model in this case to avoid numerical
  // instabilities in the LQR.
  if (std::abs(Ka) < 1e-7) {
    // System has position state and velocity input
    frc::LinearSystem<1, 1, 1> system{Matrix1d{0.0}, Matrix1d{1.0},
                                      Matrix1d{1.0}, Matrix1d{0.0}};

    frc::LinearQuadraticRegulator<1, 1> controller{
        system, {params.qp}, {params.r}, preset.period};
    controller.LatencyCompensate(system, preset.period,
                                 preset.measurementDelay);

    return {Kv * controller.K(0, 0) * preset.outputConversionFactor, 0.0};
  }

  auto system = frc::LinearSystemId::IdentifyPositionSystem<units::meters>(
      Kv_t{Kv}, Ka_t{Ka});

  frc::LinearQuadraticRegulator<2, 1> controller{
      system, {params.qp, params.qv}, {params.r}, preset.period};
  controller.LatencyCompensate(system, preset.period, preset.measurementDelay);

  return {controller.K(0, 0) * preset.outputConversionFactor,
          controller.K(0, 1) * preset.outputConversionFactor /
              (preset.normalized ? 1 : units::second_t{preset.period}.value())};
}

FeedbackGains sysid::CalculateVelocityFeedbackGains(
    const FeedbackControllerPreset& preset, const LQRParameters& params,
    double Kv, double Ka, double encFactor) {
  if (!std::isfinite(Kv) || !std::isfinite(Ka)) {
    return {0.0, 0.0};
  }

  // If acceleration for velocity control requires no effort, the feedback
  // control gains approach zero. We special-case it here to avoid numerical
  // instabilities in LQR.
  if (std::abs(Ka) < 1E-7) {
    return {0.0, 0.0};
  }

  auto system = frc::LinearSystemId::IdentifyVelocitySystem<units::meters>(
      Kv_t{Kv}, Ka_t{Ka});
  frc::LinearQuadraticRegulator<1, 1> controller{
      system, {params.qv}, {params.r}, preset.period};
  controller.LatencyCompensate(system, preset.period, preset.measurementDelay);

  return {controller.K(0, 0) * preset.outputConversionFactor /
              (preset.outputVelocityTimeFactor * encFactor),
          0.0};
}
