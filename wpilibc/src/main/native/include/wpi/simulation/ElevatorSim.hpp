// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>

#include "wpi/math/system/DCMotor.hpp"
#include "wpi/simulation/LinearSystemSim.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/mass.hpp"
#include "wpi/units/velocity.hpp"

namespace wpi::sim {
/**
 * Represents a simulated elevator mechanism.
 */
class ElevatorSim : public LinearSystemSim<2, 1, 2> {
 public:
  template <typename Distance>
  using Velocity_t = wpi::units::unit<wpi::units::compound_conversion_factor<
      Distance, wpi::units::inverse<wpi::units::seconds_>>>;

  template <typename Distance>
  using Acceleration_t =
      wpi::units::unit<wpi::units::compound_conversion_factor<
          wpi::units::compound_conversion_factor<
              Distance, wpi::units::inverse<wpi::units::seconds_>>,
          wpi::units::inverse<wpi::units::seconds_>>>;

  /**
   * Constructs a simulated elevator mechanism.
   *
   * @param plant The linear system that represents the elevator. This system
   *     can be created with wpi::math::Models::ElevatorFromPhysicalConstants().
   * @param gearbox The type of and number of motors in your elevator gearbox.
   * @param minHeight The minimum allowed height of the elevator.
   * @param maxHeight The maximum allowed height of the elevator.
   * @param simulateGravity Whether gravity should be simulated or not.
   * @param startingHeight The starting height of the elevator.
   * @param measurementStdDevs The standard deviation of the measurements.
   */
  ElevatorSim(const wpi::math::LinearSystem<2, 1, 2>& plant,
              const wpi::math::DCMotor& gearbox, wpi::units::meters<> minHeight,
              wpi::units::meters<> maxHeight, bool simulateGravity,
              wpi::units::meters<> startingHeight,
              const std::array<double, 2>& measurementStdDevs = {0.0, 0.0});

  /**
   * Constructs a simulated elevator mechanism.
   *
   * @param gearbox The type of and number of motors in your elevator gearbox.
   * @param gearing The gearing of the elevator (numbers greater than 1
   *     represent reductions).
   * @param carriageMass The mass of the elevator carriage.
   * @param drumRadius The radius of the drum that your cable is wrapped around.
   * @param minHeight The minimum allowed height of the elevator.
   * @param maxHeight The maximum allowed height of the elevator.
   * @param simulateGravity Whether gravity should be simulated or not.
   * @param startingHeight The starting height of the elevator.
   * @param measurementStdDevs The standard deviation of the measurements.
   */
  ElevatorSim(const wpi::math::DCMotor& gearbox, double gearing,
              wpi::units::kilograms<> carriageMass,
              wpi::units::meters<> drumRadius, wpi::units::meters<> minHeight,
              wpi::units::meters<> maxHeight, bool simulateGravity,
              wpi::units::meters<> startingHeight,
              const std::array<double, 2>& measurementStdDevs = {0.0, 0.0});

  /**
   * Constructs a simulated elevator mechanism.
   *
   * @param kV The velocity gain.
   * @param kA The acceleration gain.
   * @param gearbox The type of and number of motors in your elevator gearbox.
   * @param minHeight The minimum allowed height of the elevator.
   * @param maxHeight The maximum allowed height of the elevator.
   * @param simulateGravity Whether gravity should be simulated or not.
   * @param startingHeight The starting height of the elevator.
   * @param measurementStdDevs The standard deviation of the measurements.
   */
  template <typename Distance>
    requires std::same_as<wpi::units::meters_, Distance> ||
             std::same_as<wpi::units::radians_, Distance>
  ElevatorSim(decltype(1_V / Velocity_t<Distance>(1)) kV,
              decltype(1_V / Acceleration_t<Distance>(1)) kA,
              const wpi::math::DCMotor& gearbox, wpi::units::meters<> minHeight,
              wpi::units::meters<> maxHeight, bool simulateGravity,
              wpi::units::meters<> startingHeight,
              const std::array<double, 2>& measurementStdDevs = {0.0, 0.0});
  using LinearSystemSim::SetState;

  /**
   * Sets the elevator's state. The new position will be limited between the
   * minimum and maximum allowed heights.
   *
   * @param position The new position
   * @param velocity The new velocity
   */
  void SetState(wpi::units::meters<> position,
                wpi::units::meters_per_second<> velocity);

  /**
   * Returns whether the elevator would hit the lower limit.
   *
   * @param elevatorHeight The elevator height.
   * @return Whether the elevator would hit the lower limit.
   */
  bool WouldHitLowerLimit(wpi::units::meters<> elevatorHeight) const;

  /**
   * Returns whether the elevator would hit the upper limit.
   *
   * @param elevatorHeight The elevator height.
   * @return Whether the elevator would hit the upper limit.
   */
  bool WouldHitUpperLimit(wpi::units::meters<> elevatorHeight) const;

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
   * Returns the position of the elevator.
   *
   * @return The position of the elevator.
   */
  wpi::units::meters<> GetPosition() const;

  /**
   * Returns the velocity of the elevator.
   *
   * @return The velocity of the elevator.
   */
  wpi::units::meters_per_second<> GetVelocity() const;

  /**
   * Returns the elevator current draw.
   *
   * @return The elevator current draw.
   */
  wpi::units::amperes<> GetCurrentDraw() const;

  /**
   * Sets the input voltage for the elevator.
   *
   * @param voltage The input voltage.
   */
  void SetInputVoltage(wpi::units::volts<> voltage);

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
                                wpi::units::seconds<> dt) override;

 private:
  wpi::math::DCMotor m_gearbox;
  wpi::units::meters<> m_minHeight;
  wpi::units::meters<> m_maxHeight;
  bool m_simulateGravity;
};
}  // namespace wpi::sim
