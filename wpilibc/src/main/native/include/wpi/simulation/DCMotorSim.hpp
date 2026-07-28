// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/system/DCMotor.hpp"
#include "wpi/math/system/LinearSystem.hpp"
#include "wpi/simulation/LinearSystemSim.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_acceleration.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/moment_of_inertia.hpp"
#include "wpi/units/torque.hpp"

namespace wpi::sim {
/**
 * Represents a simulated DC motor mechanism.
 */
class DCMotorSim : public LinearSystemSim<2, 1, 2> {
 public:
  /**
   * Creates a simulated DC motor mechanism.
   *
   * @param plant The linear system representing the DC motor. This system can
   *     be created with
   *     wpi::math::Models::SingleJointedArmFromPhysicalConstants() or
   *     wpi::math::Models::SingleJointedArmFromSysId().
   * @param gearbox The type of and number of motors in the DC motor gearbox.
   * @param measurementStdDevs The standard deviation of the measurement noise.
   * @throws std::domain_error if the plant's A(1, 1) or B(1, 0) entry is zero,
   *     which leaves the gearing and moment of inertia undetermined. A(1, 1) is
   *     zero for a plant built from SysId constants with kV = 0.
   */
  DCMotorSim(const wpi::math::LinearSystem<2, 1, 2>& plant,
             const wpi::math::DCMotor& gearbox,
             const std::array<double, 2>& measurementStdDevs = {0.0, 0.0});

  using LinearSystemSim::SetState;

  /**
   * Sets the state of the DC motor.
   *
   * @param angularPosition The new position
   * @param angularVelocity The new velocity
   */
  void SetState(wpi::units::radian_t angularPosition,
                wpi::units::radians_per_second_t angularVelocity);

  /**
   * Sets the DC motor's angular position.
   *
   * @param angularPosition The new position in radians.
   */
  void SetAngle(wpi::units::radian_t angularPosition);

  /**
   * Sets the DC motor's angular velocity.
   *
   * @param angularVelocity The new velocity in radians per second.
   */
  void SetAngularVelocity(wpi::units::radians_per_second_t angularVelocity);

  /**
   * Returns the DC motor position.
   *
   * @return The DC motor position.
   */
  wpi::units::radian_t GetAngularPosition() const;

  /**
   * Returns the DC motor velocity.
   *
   * @return The DC motor velocity.
   */
  wpi::units::radians_per_second_t GetAngularVelocity() const;

  /**
   * Returns the DC motor acceleration.
   *
   * @return The DC motor acceleration
   */
  wpi::units::radians_per_second_squared_t GetAngularAcceleration() const;

  /**
   * Returns the DC motor torque.
   *
   * @return The DC motor torque
   */
  wpi::units::newton_meter_t GetTorque() const;

  /**
   * Returns the DC motor current draw.
   *
   * This is the current drawn from the battery, which differs from the current
   * through the motor by the duty cycle the motor controller is applying. A
   * negative value means the motor is regenerating and returning current to the
   * battery.
   *
   * @return The DC motor current draw.
   */
  wpi::units::ampere_t GetCurrentDraw() const;

  /**
   * Gets the input voltage for the DC motor.
   *
   * @return The DC motor input voltage.
   */
  wpi::units::volt_t GetInputVoltage() const;

  /**
   * Sets the input voltage for the DC motor.
   *
   * @param voltage The input voltage.
   */
  void SetInputVoltage(wpi::units::volt_t voltage);

  /**
   * Returns the gearbox for the DC motor.
   *
   * @return The DC motor's gearbox.
   */
  const wpi::math::DCMotor& GetGearbox() const;

  /**
   * Returns the gear ratio of the DC motor.
   *
   * @return The DC motor's gear ratio.
   */
  double GetGearing() const;

  /**
   * Returns the moment of inertia of the DC motor.
   *
   * @return The DC motor's moment of inertia.
   */
  wpi::units::kilogram_square_meter_t GetJ() const;

 private:
  wpi::math::DCMotor m_gearbox;
  double m_gearing;
  wpi::units::kilogram_square_meter_t m_j;
};
}  // namespace wpi::sim
