// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/angle.h>
#include <units/angular_acceleration.h>
#include <units/angular_velocity.h>
#include <units/moment_of_inertia.h>
#include <units/torque.h>
#include <units/voltage.h>

#include "frc/simulation/LinearSystemSim.h"
#include "frc/system/LinearSystem.h"
#include "frc/system/plant/DCMotor.h"

namespace frc::sim {
/**
 * Represents a simulated angular mechanism controlled by a DC motor.
 */
class AngularMechanismSim : public LinearSystemSim<2, 1, 2> {
 public:
  /**
   * Creates a simulated angular mechanism controller by a DC motor.
   *
   * @param plant              The linear system representing the angular mechanism. This
   *                           system can be created with
   *                           LinearSystemId::CreateAngularSystem().
   * @param gearbox            The type of and number of motors in the DC motor
   * gearbox.
   * @param measurementStdDevs The standard deviation of the measurement noise.
   */
  AngularMechanismSim(const LinearSystem<2, 1, 2>& plant, const DCMotor& gearbox,
             const std::array<double, 2>& measurementStdDevs = {0.0, 0.0});

  using LinearSystemSim::SetState;

  /**
   * Returns the gear ratio of the mechanism.
   * 
   * @return the mechanism's gear ratio.
  */
 double GetGearing(){ return m_gearing; }

 /**
  * Returns the moment of inertia of the mechanism.
  * 
  * @return the mechanism's moment of inertia.
 */
  units::kilogram_square_meter_t GetJ() const;


  /**
   * Sets the state of the mechanism.
   *
   * @param angularPosition The new position
   * @param angularVelocity The new velocity
   */
  void SetState(units::radian_t angularPosition,
                units::radians_per_second_t angularVelocity);

  /**
   * Sets the position of the mechanism.
   * 
   * @param angularPosition The new position
  */
  void SetPosition(units::radian_t angularPosition);

  /**
   * Sets the velocity of the mechanism.
   * 
   * @param angularVelocity The new position
  */
  void SetVelocity(units::radians_per_second_t angularVelocity);

  /**
   * Returns the position of the mechanism.
   *
   * @return The mechanism's position.
   */
  units::radian_t GetAngularPosition() const;

  /**
   * Returns the velocity of the mechanism.
   *
   * @return The mechanism's velocity.
   */
  units::radians_per_second_t GetAngularVelocity() const;

  /**
   * Returns the acceleration of the mechanism.
   * 
   * @return the mechanism's acceleration.
  */
 units::radians_per_second_squared_t GetAngularAcceleration() const;

 /**
  * Returns the torque of the mechanism.
  * 
  * @return the mechanism's torque.
 */
units::newton_meter_t GetTorque() const;

  /**
   * Returns the mechanism's DC motor current draw.
   *
   * @return The mechanism's DC motor current draw.
   */
  units::ampere_t GetCurrentDraw() const;

  /**
   * Sets the input voltage for mechanism's DC motor.
   *
   * @param voltage The input voltage.
   */
  void SetInputVoltage(units::volt_t voltage);

  /**
   * Gets the input voltage for the mechanism's DC motor.
   * 
   * @return the mechanism's DC motor input voltage.
  */
 units::volt_t GetInputVoltage() const;

 private:
  DCMotor m_gearbox;
  double m_gearing;
  LinearSystem<2, 1, 2> m_plant;
};
}  // namespace frc::sim
