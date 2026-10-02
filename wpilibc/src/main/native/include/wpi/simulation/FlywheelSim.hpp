// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/system/DCMotor.hpp"
#include "wpi/math/system/LinearSystem.hpp"
#include "wpi/simulation/LinearSystemSim.hpp"
#include "wpi/units/angular_acceleration.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/moment_of_inertia.hpp"
#include "wpi/units/torque.hpp"

namespace wpi::sim {
/**
 * Represents a simulated flywheel mechanism.
 */
class FlywheelSim : public LinearSystemSim<1, 1, 1> {
 public:
  /**
   * Creates a simulated flywheel mechanism.
   *
   * @param plant The linear system representing the flywheel. This system can
   *     be created with wpi::math::Models::FlywheelFromPhysicalConstants() or
   *     wpi::math::Models::FlywheelFromSysId().
   * @param gearbox The type of and number of motors in the flywheel gearbox.
   * @param measurementStdDevs The standard deviation of the measurement noise.
   */
  FlywheelSim(const wpi::math::LinearSystem<1, 1, 1>& plant,
              const wpi::math::DCMotor& gearbox,
              const std::array<double, 1>& measurementStdDevs = {0.0});

  using LinearSystemSim::SetState;

  /**
   * Sets the flywheel's angular velocity.
   *
   * @param velocity The new velocity
   */
  void SetVelocity(wpi::units::radians_per_second_t velocity);

  /**
   * Returns the flywheel's velocity.
   *
   * @return The flywheel's velocity.
   */
  wpi::units::radians_per_second_t GetAngularVelocity() const;

  /**
   * Returns the flywheel's acceleration.
   *
   * @return The flywheel's acceleration
   */
  wpi::units::radians_per_second_squared_t GetAngularAcceleration() const;

  /**
   * Returns the flywheel's torque.
   *
   * @return The flywheel's torque
   */
  wpi::units::newton_meter_t GetTorque() const;

  /**
   * Returns the flywheel's current draw.
   *
   * @return The flywheel's current draw.
   */
  wpi::units::ampere_t GetCurrentDraw() const;

  /**
   * Gets the input voltage for the flywheel.
   *
   * @return The flywheel input voltage.
   */
  wpi::units::volt_t GetInputVoltage() const;

  /**
   * Sets the input voltage for the flywheel.
   *
   * @param voltage The input voltage.
   */
  void SetInputVoltage(wpi::units::volt_t voltage);

  /**
   * Returns the gearbox.
   */
  wpi::math::DCMotor Gearbox() const { return m_gearbox; }

  /**
   * Returns the gearing;
   */
  double Gearing() const { return m_gearing; }

  /**
   * Returns the moment of inertia
   */
  wpi::units::kilogram_square_meter_t J() const { return m_j; }

 private:
  wpi::math::DCMotor m_gearbox;
  double m_gearing;
  wpi::units::kilogram_square_meter_t m_j;
};
}  // namespace wpi::sim
