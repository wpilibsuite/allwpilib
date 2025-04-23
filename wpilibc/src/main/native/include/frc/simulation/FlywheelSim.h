// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/angular_acceleration.h>
#include <units/angular_velocity.h>
#include <units/torque.h>

#include "frc/RobotController.h"
#include "frc/simulation/LinearSystemSim.h"
#include "frc/system/plant/DCMotor.h"
#include "frc/system/plant/Gearbox.h"
#include "frc/system/plant/LinearSystemId.h"

namespace frc::sim {
/**
 * Represents a simulated flywheel mechanism.
 */
template <typename Input>
  requires units::current_unit<Input> || units::voltage_unit<Input>
class FlywheelSim : public LinearSystemSim<1, 1, 1> {
 public:
  using Input_t = units::unit_t<Input>;
  /**
   * Creates a simulated flywheel mechanism.
   *
   * @param gearbox            The flywheel gearbox.
   * @param measurementStdDevs The standard deviation of the measurement noise.
   */
  explicit FlywheelSim(const Gearbox& gearbox, const Input_t maxInput,
                       const std::array<double, 1>& measurementStdDevs = {0.0})
      : LinearSystemSim(LinearSystemId::FlywheelSystem(gearbox),
                        measurementStdDevs),
        m_gearbox(gearbox),
        m_maxInput(maxInput.value()) {}

  using LinearSystemSim::SetInput;
  using LinearSystemSim::SetState;

  /**
   * Sets the flywheel's angular velocity.
   *
   * @param velocity The new velocity
   */
  void SetVelocity(units::radians_per_second_t velocity) {
    LinearSystemSim::SetState(Vectord<1>{velocity.value()});
  }

  /**
   * Returns the flywheel's velocity.
   *
   * @return The flywheel's velocity.
   */
  [[nodiscard]]
  units::radians_per_second_t GetAngularVelocity() const {
    return units::radians_per_second_t{GetOutput(0)};
  }

  /**
   * Returns the flywheel's acceleration.
   *
   * @return The flywheel's acceleration
   */
  [[nodiscard]]
  units::radians_per_second_squared_t GetAngularAcceleration() const {
    return units::radians_per_second_squared_t{
        (m_plant.A() * m_x + m_plant.B() * m_u)(0, 0)};
  }

  /**
   * Returns the flywheel's torque.
   *
   * @return The flywheel's torque
   */
  [[nodiscard]]
  units::newton_meter_t GetTorque() const {
    return units::newton_meter_t{GetAngularAcceleration().value() *
                                 m_gearbox.J.value()};
  }

  /**
   * Returns the flywheel's current draw.
   *
   * @return The flywheel's current draw.
   */
  [[nodiscard]]
  units::ampere_t GetCurrent() const {
    // I = V / R - omega / (Kv * R)
    // Reductions are greater than 1, so a reduction of 10:1 would mean the
    // motor is spinning 10x faster than the output.
    return m_gearbox.Current(units::radians_per_second_t{m_x(0)},
                             units::volt_t{m_u(0)});
  }

  /**
   * Gets the input voltage for the flywheel.
   *
   * @return The flywheel input voltage.
   */
  [[nodiscard]]
  units::volt_t GetVoltage() const {
    return units::volt_t{GetInput(0)};
  }

  /**
   * Sets the input for the flywheel.
   *
   * @param input The input.
   */
  void SetInput(const Input_t input) {
    SetInput(Vectord<1>{input.value()});
    ClampInput(m_maxInput);
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
  const double m_maxInput;
};

}  // namespace frc::sim
