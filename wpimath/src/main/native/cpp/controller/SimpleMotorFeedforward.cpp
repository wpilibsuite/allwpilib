// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/SimpleMotorFeedforward.h"

using namespace frc;

units::unit_t<Velocity> SimpleMotorFeedforward::MaxAchievableVelocity(
      units::volt_t ks,
      units::unit_t<kv_unit> kv,
      units::unit_t<ka_unit> ka,
      units::volt_t maxVoltage,
      units::unit_t<Acceleration> acceleration) {
    // Assume max velocity is positive
    return (maxVoltage - ka - ka * acceleration) / kv;
}

units::unit_t<Velocity> SimpleMotorFeedforward::MaxAchievableVelocity(
      SimpleMotorFeedforward feedforward,
      units::volt_t maxVoltage,
      units::unit_t<Acceleration> acceleration) {
    // Assume max velocity is positive
    return MaxAchievableVelocity(feedforward.GetKs(), feedforward.GetKv(), feedforward.GetKa, maxVoltage, acceleration);
}

units::unit_t<Velocity> SimpleMotorFeedforward::MinAchievableVelocity(
      units::volt_t ks,
      units::unit_t<kv_unit> kv,
      units::unit_t<ka_unit> ka,
      units::volt_t maxVoltage,
      units::unit_t<Acceleration> acceleration) {
    // Assume min velocity is positive, ks flips sign
    return MaxAchievableVelocity(-ks, kv, ka, -maxVoltage, acceleration);
}

units::unit_t<Velocity> SimpleMotorFeedforward::MinAchievableVelocity(
      SimpleMotorFeedforward feedforward,
      units::volt_t maxVoltage,
      units::unit_t<Acceleration> acceleration) {
    // Assume max velocity is positive
    return MinAchievableVelocity(feedforward.getKs(), feedforward.getKv(), feedforward.getKa(), maxVoltage, acceleration);
}    

units::unit_t<Acceleration> SimpleMotorFeedforward::MaxAchievableAcceleration(
      units::volt_t ks,
      units::unit_t<kv_unit> kv,
      units::unit_t<ka_unit> ka,
      units::volt_t maxVoltage,
      units::unit_t<Velocity> velocity) {
    return (maxVoltage - ks * wpi::sgn(velocity) - kv * velocity) / ka;
}

units::unit_t<Acceleration> SimpleMotorFeedforward::MaxAchievableAcceleration(
      SimpleMotorFeedforward feedforward,
      units::volt_t maxVoltage,
      units::unit_t<Velocity> velocity) {
    return MaxAchievableAcceleration(feedforward.getKs(), feedforward.getKv(), feedforward.getKa(), maxVoltage, velocity);
}

units::unit_t<Acceleration> SimpleMotorFeedforward::MinAchievableAcceleration(
      units::volt_t ks,
      units::unit_t<kv_unit> kv,
      units::unit_t<ka_unit> ka,
      units::volt_t maxVoltage,
      units::unit_t<Velocity> velocity) {
    return MaxAchievableAcceleration(-maxVoltage, velocity);
}

units::unit_t<Acceleration> SimpleMotorFeedforward::MinAchievableAcceleration(
      SimpleMotorFeedforward feedforward,
      units::volt_t maxVoltage,
      units::unit_t<Velocity> velocity) {
    return MinAchievableAcceleration(feedforward.getKs(), feedforward.getKv(), feedforward.getKa(), maxVoltage, velocity);
} 












