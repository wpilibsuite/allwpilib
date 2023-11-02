// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/angle.h>
#include <units/angular_velocity.h>
#include <units/moment_of_inertia.h>

#include "frc/simulation/LinearSystemSim.h"
#include "frc/system/LinearSystem.h"
#include "frc/system/plant/DCMotor.h"

namespace frc::sim {
/**
 * Represents a simulated DC motor mechanism.
 */
class DCMotorSim : public LinearSystemSim<2, 1, 2> {
 public:
   template <typename Distance>
  using Velocity_t = units::unit_t<
      units::compound_unit<Distance, units::inverse<units::seconds>>>;

  template <typename Distance>
  using Acceleration_t = units::unit_t<units::compound_unit<
      units::compound_unit<Distance, units::inverse<units::seconds>>,
      units::inverse<units::seconds>>>;
  /**
   * Creates a simulated DC motor mechanism.
   *
   * @param plant              The linear system representing the DC motor. This
   *                           system can be created with
   *                           LinearSystemId::DCMotorSystem().
   * @param gearbox            The type of and number of motors in the DC motor
   * gearbox.
   * @param measurementStdDevs The standard deviation of the measurement noise.
   */
  DCMotorSim(const LinearSystem<2, 1, 2>& plant, const DCMotor& gearbox,
             const std::array<double, 2>& measurementStdDevs = {0.0, 0.0});

  /**
   * Creates a simulated DC motor mechanism.
   *
   * @param gearbox            The type of and number of motors in the DC motor
   * gearbox.
   * @param gearing            The gearing of the DC motor (numbers greater than
   * 1 represent reductions).
   * @param moi                The moment of inertia of the DC motor.
   * @param measurementStdDevs The standard deviation of the measurement noise.
   */
  DCMotorSim(const DCMotor& gearbox, double gearing,
             units::kilogram_square_meter_t moi,
             const std::array<double, 2>& measurementStdDevs = {0.0, 0.0});

  /**
   * Constructs a simulated elevator mechanism.
   *
   * @param kV                 The velocity gain.
   * @param kA                 The acceleration gain.
   * @param gearbox            The type of and number of motors in the
   *                           DC motor gearbox.
   */
  template <typename Distance>
    requires std::same_as<units::radian, Distance>
  DCMotorSim(decltype(1_V / Velocity_t<Distance>(1)) kV,
              decltype(1_V / Acceleration_t<Distance>(1)) kA,
              const DCMotor& gearbox, 
              const std::array<double, 2>& measurementStdDevs = {0.0, 0.0});

  using LinearSystemSim::SetState;

  /**
   * Sets the state of the DC motor.
   *
   * @param angularPosition The new position
   * @param angularVelocity The new velocity
   */
  void SetState(units::radian_t angularPosition,
                units::radians_per_second_t angularVelocity);

  /**
   * Returns the DC motor position.
   *
   * @return The DC motor position.
   */
  units::radian_t GetAngularPosition() const;

  /**
   * Returns the DC motor velocity.
   *
   * @return The DC motor velocity.
   */
  units::radians_per_second_t GetAngularVelocity() const;

  /**
   * Returns the DC motor current draw.
   *
   * @return The DC motor current draw.
   */
  units::ampere_t GetCurrentDraw() const override;

  /**
   * Sets the input voltage for the DC motor.
   *
   * @param voltage The input voltage.
   */
  void SetInputVoltage(units::volt_t voltage);

 private:
  DCMotor m_gearbox;
};
}  // namespace frc::sim
