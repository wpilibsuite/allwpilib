// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/SimpleMotorFeedforward.h"

#include <wpi/MathExtras.h>

using namespace frc;

template <class Distance>
SimpleMotorFeedforward<Distance>::SimpleMotorFeedforward(
    units::volt_t kS, units::unit_t<kv_unit> kV, units::unit_t<ka_unit> kA,
    units::second_t periodSeconds)
    : kS(kS), kV(kV), kA(kA) {
  if (kV.value() < 0) {
    wpi::math::MathSharedStore::ReportError(
        "kV must be a non-negative number, got {}!", kV.value());
    kV = units::unit_t<kv_unit>{0};
    wpi::math::MathSharedStore::ReportWarning("kV defaulted to 0.");
  }
  if (kA.value() < 0) {
    wpi::math::MathSharedStore::ReportError(
        "kA must be a non-negative number, got {}!", kA.value());
    kA = units::unit_t<ka_unit>{0};
    wpi::math::MathSharedStore::ReportWarning("kA defaulted to 0;");
  }
  if (periodSeconds.value() < 0) {
    wpi::math::MathSharedStore::ReportError(
        "period must be a positive number, got {}!", periodSeconds.value());
    periodSeconds = units::second_t{0.020};
    wpi::math::MathSharedStore::ReportWarning("period defaulted to 0.020;");
  }
  if (kA.value() != 0.0) {
    auto plant = LinearSystemId::IdentifyVelocitySystem<Distance>(kV, kA);
    feedforward = LinearPlantInversionFeedforward<1, 1>{plant, periodSeconds};
  }
}