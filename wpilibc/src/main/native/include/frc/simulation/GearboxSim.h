// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/angle.h>
#include <units/angular_acceleration.h>
#include <units/angular_velocity.h>
#include <units/torque.h>

#include "frc/simulation/LinearSystemSim.h"
#include "frc/system/LinearSystem.h"
#include "frc/system/plant/DCMotor.h"
#include "frc/system/plant/Gearbox.h"
#include "frc/system/plant/LinearSystemId.h"

namespace frc::sim {
/**
 * Represents a simulated Gearbox mechanism.
 */
template <typename Input>
  requires units::current_unit<Input> || units::voltage_unit<Input>
class GearboxSim : public LinearSystemSim<2, 1, 2> {
 public:
  using Input_t = units::unit_t<Input>;
  /**
   * Creates a simulated DC motor mechanism.
   *
   * @param gearbox            The system's gearbox.
   * @param measurementStdDevs The standard deviation of the measurement noise.
   */
  explicit GearboxSim(const Gearbox& gearbox,
                      const std::array<double, 2>& measurementStdDevs = {0.0,
                                                                         0.0})
      : LinearSystemSim(LinearSystemId::GearboxSystem(gearbox),
                        measurementStdDevs),
        m_gearbox(gearbox) {}

  using LinearSystemSim::SetState;

  /**
   * Sets the state of the DC motor.
   *
   * @param angularPosition The new position
   * @param angularVelocity The new velocity
   */
  void SetState(const units::radian_t angularPosition,
                const units::radians_per_second_t angularVelocity) {
    SetState(Vectord<2>{angularPosition, angularVelocity});
  }

  /**
   * Sets the DC motor's angular position.
   *
   * @param angularPosition The new position in radians.
   */
  void SetAngle(const units::radian_t angularPosition) {
    SetState(angularPosition, GetAngularVelocity());
  }

  /**
   * Sets the DC motor's angular velocity.
   *
   * @param angularVelocity The new velocity in radians per second.
   */
  void SetAngularVelocity(const units::radians_per_second_t angularVelocity) {
    SetState(GetAngularPosition(), angularVelocity);
  }

  /**
   * Returns the DC motor position.
   *
   * @return The DC motor position.
   */
  [[nodiscard]]
  units::radian_t GetAngularPosition() const {
    return units::radian_t{GetOutput(0)};
  }

  /**
   * Returns the DC motor velocity.
   *
   * @return The DC motor velocity.
   */
  [[nodiscard]]
  units::radians_per_second_t GetAngularVelocity() const {
    return units::radians_per_second_t{GetOutput(1)};
  }

  /**
   * Returns the DC motor acceleration.
   *
   * @return The DC motor acceleration
   */
  [[nodiscard]]
  units::radians_per_second_squared_t GetAngularAcceleration() const {
    return units::radians_per_second_squared_t{
        (m_plant.A() * m_x + m_plant.B() * m_u)(1, 0)};
  }

  /**
   * Returns the DC motor torque.
   *
   * @return The DC motor torque
   */
  [[nodiscard]]
  units::newton_meter_t GetTorque() const {
    return units::newton_meter_t{GetAngularAcceleration().value() *
                                 m_gearbox.J.value()};
  }

  /**
   * Returns the DC motor current draw.
   *
   * @return The DC motor current draw.
   */
  [[nodiscard]]
  units::ampere_t GetCurrent() const {
    // I = V / R - omega / (Kv * R)
    // Reductions are greater than 1, so a reduction of 10:1 would mean the
    // motor is spinning 10x faster than the output.
    return m_gearbox.Current(units::radians_per_second_t{m_x(1)},
                             units::volt_t{m_u(0)});
  }

  /**
   * Gets the input voltage for the DC motor.
   *
   * @return The DC motor input voltage.
   */
  [[nodiscard]]
  units::volt_t GetVoltage() const {
    return m_gearbox.Voltage(GetAngularAcceleration(),
                             units::radians_per_second_t{m_x(1)});
  }

  /**
   * Sets the input voltage for the DC motor.
   *
   * @param voltage The input voltage.
   */
  void SetInputVoltage(const units::volt_t voltage) {
    SetInput(Vectord<1>{voltage.value()});
    ClampInput(frc::RobotController::GetBatteryVoltage().value());
  }

  /**
   * Returns the gearbox.
   */
  [[nodiscard]]
  const Gearbox& GetGearbox() const {
    return m_gearbox;
  }

 private:
  const Gearbox m_gearbox;
};
}  // namespace frc::sim
