// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>

#include "wpi/math/system/DCMotor.hpp"
#include "wpi/simulation/LinearSystemSim.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/force.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/mass.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"

namespace wpi::sim {
/**
 * Represents a simulated elevator mechanism.
 *
 * Gravity is supplied as a gravity gain kG rather than assumed to act along the
 * full travel, so this also models horizontal rack-and-pinion mechanisms
 * (kG = 0), canted elevators, and elevators with a counterbalance.
 */
class ElevatorSim : public LinearSystemSim<2, 1, 2> {
 public:
  /**
   * Constructs a simulated elevator mechanism.
   *
   * @param plant The linear system that represents the elevator. This system
   *     can be created with wpi::math::Models::ElevatorFromPhysicalConstants()
   *     or wpi::math::Models::ElevatorFromSysId().
   * @param gearbox The type of and number of motors in your elevator gearbox.
   * @param gearing The gear ratio from the motors to the elevator drum, as
   *     input over output (greater than 1 is a reduction).
   * @param kG The gravity gain. Minimum voltage needed to hold an elevator in
   *     place. Zero for a horizontal mechanism, and less than the vertical
   *     value for a canted one or one with a counterbalance.
   * @param minHeight The minimum allowed height of the elevator.
   * @param maxHeight The maximum allowed height of the elevator.
   * @param startingHeight The starting height of the elevator.
   * @param measurementStdDevs The standard deviation of the measurements.
   * @throws std::domain_error if gearing <= 0, minHeight > maxHeight, or the
   *     plant's A(1, 1) or B(1, 0) entry is zero, which leaves the drum radius
   *     and carriage mass undetermined. A(1, 1) is zero for a plant built from
   *     SysId constants with kV = 0.
   */
  ElevatorSim(const wpi::math::LinearSystem<2, 1, 2>& plant,
              const wpi::math::DCMotor& gearbox, double gearing,
              wpi::units::volt_t kG, wpi::units::meter_t minHeight,
              wpi::units::meter_t maxHeight, wpi::units::meter_t startingHeight,
              const std::array<double, 2>& measurementStdDevs = {0.0, 0.0});

  using LinearSystemSim::SetState;

  /**
   * Sets the elevator's state. The new position will be limited between the
   * minimum and maximum allowed heights.
   *
   * @param position The new position
   * @param velocity The new velocity
   */
  void SetState(wpi::units::meter_t position,
                wpi::units::meters_per_second_t velocity);

  /**
   * Sets the elevator's position. The new position will be limited between the
   * minimum and maximum allowed heights.
   *
   * @param position The new position
   */
  void SetPosition(wpi::units::meter_t position);

  /**
   * Sets the elevator's velocity.
   *
   * @param velocity The new velocity
   */
  void SetVelocity(wpi::units::meters_per_second_t velocity);

  /**
   * Returns whether the elevator would hit the lower limit.
   *
   * @param elevatorHeight The elevator height.
   * @return Whether the elevator would hit the lower limit.
   */
  bool WouldHitLowerLimit(wpi::units::meter_t elevatorHeight) const;

  /**
   * Returns whether the elevator would hit the upper limit.
   *
   * @param elevatorHeight The elevator height.
   * @return Whether the elevator would hit the upper limit.
   */
  bool WouldHitUpperLimit(wpi::units::meter_t elevatorHeight) const;

  /**
   * Returns whether the elevator has hit the lower limit.
   *
   * @return Whether the elevator has hit the lower limit.
   */
  bool HasHitLowerLimit() const;

  /**
   * Returns whether the elevator has hit the upper limit.
   *
   * @return Whether the elevator has hit the upper limit.
   */
  bool HasHitUpperLimit() const;

  /**
   * Returns the gearbox for the elevator.
   *
   * @return The gearbox for the elevator.
   */
  const wpi::math::DCMotor& GetGearbox() const;

  /**
   * Returns the gear ratio of the elevator's gearbox.
   *
   * @return The elevator's gearbox gear ratio.
   */
  double GetGearing() const;

  /**
   * Returns the drum radius of the elevator.
   *
   * This is derived from the plant and the gearing rather than stored as
   * given, so it reflects the radius the plant's dynamics actually imply,
   * including rope or belt pitch diameter and any cascade stage multiplier.
   *
   * @return The elevator's drum radius.
   */
  wpi::units::meter_t GetDrumRadius() const;

  /**
   * Returns the mass of the elevator carriage.
   *
   * @return The mass of the elevator carriage.
   */
  wpi::units::kilogram_t GetCarriageMass() const;

  /**
   * Returns the gravity gain of the elevator.
   *
   * @return The gravity gain.
   */
  wpi::units::volt_t GetKg() const;

  /**
   * Returns the position of the elevator.
   *
   * @return The position of the elevator.
   */
  wpi::units::meter_t GetPosition() const;

  /**
   * Returns the velocity of the elevator.
   *
   * @return The velocity of the elevator.
   */
  wpi::units::meters_per_second_t GetVelocity() const;

  /**
   * Returns the acceleration of the elevator.
   *
   * @return The acceleration of the elevator.
   */
  wpi::units::meters_per_second_squared_t GetAcceleration() const;

  /**
   * Returns the elevator's force.
   *
   * @return The elevator's force.
   */
  wpi::units::newton_t GetForce() const;

  /**
   * Returns the elevator's current draw.
   *
   * This is the current drawn from the battery, which differs from the current
   * through the motor by the duty cycle the motor controller is applying. A
   * negative value means the elevator is regenerating and returning current to
   * the battery.
   *
   * @return The elevator current draw.
   */
  wpi::units::ampere_t GetCurrentDraw() const;

  /**
   * Gets the input voltage for the elevator.
   *
   * @return The elevator's input voltage.
   */
  wpi::units::volt_t GetInputVoltage() const;

  /**
   * Sets the input voltage for the elevator.
   *
   * @param voltage The input voltage.
   */
  void SetInputVoltage(wpi::units::volt_t voltage);

 protected:
  /**
   * Updates the state estimate of the elevator.
   *
   * @param currentXhat The current state estimate.
   * @param u The system inputs (voltage).
   * @param dt The time difference between controller updates.
   */
  wpi::math::Vectord<2> UpdateX(const wpi::math::Vectord<2>& currentXhat,
                                const wpi::math::Vectord<1>& u,
                                wpi::units::second_t dt) override;

 private:
  wpi::math::DCMotor m_gearbox;
  double m_gearing;
  wpi::units::volt_t m_kG;
  wpi::units::meter_t m_drumRadius;
  wpi::units::kilogram_t m_carriageMass;
  wpi::units::meter_t m_minHeight;
  wpi::units::meter_t m_maxHeight;
};
}  // namespace wpi::sim
