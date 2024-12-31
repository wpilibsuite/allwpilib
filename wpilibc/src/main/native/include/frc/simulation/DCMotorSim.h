// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/angle.h>
#include <units/angular_acceleration.h>
#include <units/angular_velocity.h>
#include <units/moment_of_inertia.h>
#include <units/torque.h>

#include "frc/simulation/LinearSystemSim.h"
#include "frc/system/LinearSystem.h"
#include "frc/system/plant/DCMotor.h"

namespace frc::sim {
/**
 * Represents a simulated DC motor mechanism.
 */
class DCMotorSim : public LinearSystemSim<2, 1, 2> {
 public:
  /**
   * Creates a simulated DC motor mechanism.
   *
   * @param plant              The linear system representing the DC motor. This
   * system can be created with LinearSystemId::DCMotorSystem(). If
   * LinearSystemId::DCMotorSystem(kV, kA) is used, the distance unit must be
   * radians.
   * @param gearbox            The type of and number of motors in the DC motor
   * gearbox.
   * @param measurementStdDevs The standard deviation of the measurement noise.
   */
  DCMotorSim(const LinearSystem<2, 1, 2>& plant, const DCMotor& gearbox,
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
   * Sets the DC motor's angular position.
   *
   * @param angularPosition The new position in radians.
   */
  void SetAngle(units::radian_t angularPosition);

  /**
   * Sets the DC motor's angular velocity.
   *
   * @param angularVelocity The new velocity in radians per second.
   */
  void SetAngularVelocity(units::radians_per_second_t angularVelocity);

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
   * Returns the DC motor acceleration.
   *
   * @return The DC motor acceleration
   */
  units::radians_per_second_squared_t GetAngularAcceleration() const;

  /**
   * Returns the DC motor torque.
   *
   * @return The DC motor torque
   */
  units::newton_meter_t GetTorque() const;

  /**
   * Returns the DC motor current draw.
   *
   * @return The DC motor current draw.
   */
  units::ampere_t GetCurrentDraw() const;

  /**
   * Gets the input voltage for the DC motor.
   *
   * @return The DC motor input voltage.
   */
  units::volt_t GetInputVoltage() const;

  /**
   * Sets the input voltage for the DC motor.
   *
   * @param voltage The input voltage.
   */
  void SetInputVoltage(units::volt_t voltage);

  /**
   * Returns the gearbox.
   */
  const DCMotor& GetGearbox() const;

  /**
   * Returns the gearing;
   */
  double GetGearing() const;

  /**
   * Returns the moment of inertia
   */
  units::kilogram_square_meter_t GetJ() const;

 private:
  DCMotor m_gearbox;
  double m_gearing;
  units::kilogram_square_meter_t m_j;
};
}  // namespace frc::sim
