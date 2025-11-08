// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/sysid/analysis/FeedbackAnalysis.hpp"

#include <cmath>

#include "wpi/math/controller/LinearQuadraticRegulator.hpp"
#include "wpi/math/system/LinearSystem.hpp"
#include "wpi/math/system/plant/LinearSystemId.hpp"
#include "wpi/sysid/analysis/FeedbackControllerPreset.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"

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
    wpi::math::LinearSystem<1, 1, 1> system{Matrix1d{0.0}, Matrix1d{1.0},
                                            Matrix1d{1.0}, Matrix1d{0.0}};

    wpi::math::LinearQuadraticRegulator<1, 1> controller{
        system, {params.qp}, {params.r}, preset.period};
    controller.LatencyCompensate(system, preset.period,
                                 preset.measurementDelay);

    return {Kv * controller.K(0, 0) * preset.outputConversionFactor, 0.0};
  }

  auto system =
      wpi::math::LinearSystemId::IdentifyPositionSystem<wpi::units::meters>(
          Kv_t{Kv}, Ka_t{Ka});

  wpi::math::LinearQuadraticRegulator<2, 1> controller{
      system, {params.qp, params.qv}, {params.r}, preset.period};
  controller.LatencyCompensate(system, preset.period, preset.measurementDelay);

  return {
      controller.K(0, 0) * preset.outputConversionFactor,
      controller.K(0, 1) * preset.outputConversionFactor /
          (preset.normalized ? 1
                             : wpi::units::second_t{preset.period}.value())};
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

  auto system =
      wpi::math::LinearSystemId::IdentifyVelocitySystem<wpi::units::meters>(
          Kv_t{Kv}, Ka_t{Ka});
  wpi::math::LinearQuadraticRegulator<1, 1> controller{
      system, {params.qv}, {params.r}, preset.period};
  controller.LatencyCompensate(system, preset.period, preset.measurementDelay);

  return {controller.K(0, 0) * preset.outputConversionFactor /
              (preset.outputVelocityTimeFactor * encFactor),
          0.0};
}
