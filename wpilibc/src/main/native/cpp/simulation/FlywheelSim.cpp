// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/FlywheelSim.hpp"

#include <algorithm>
#include <stdexcept>

#include "wpi/system/RobotController.hpp"

using namespace wpi;
using namespace wpi::sim;

FlywheelSim::FlywheelSim(const wpi::math::LinearSystem<1, 1, 1>& plant,
                         const wpi::math::DCMotor& gearbox,
                         const std::array<double, 1>& measurementStdDevs)
    : LinearSystemSim<1, 1, 1>(plant, measurementStdDevs),
      m_gearbox(gearbox),
      // By theorem 6.10.1 of
      // https://file.tavsys.net/control/controls-engineering-in-frc.pdf, the
      // flywheel state-space model is:
      //
      //   dx/dt = -G²Kₜ/(KᵥRJ)x + (GKₜ)/(RJ)u
      //   A = -G²Kₜ/(KᵥRJ)
      //   B = GKₜ/(RJ)
      //
      // Solve for G.
      //
      //   A/B = -G/Kᵥ
      //   G = -KᵥA/B
      //
      // Solve for J.
      //
      //   B = GKₜ/(RJ)
      //   J = GKₜ/(RB)
      m_gearing(-gearbox.Kv.value() * m_plant.A(0, 0) / m_plant.B(0, 0)),
      m_j(m_gearing * gearbox.Kt.value() /
          (gearbox.R.value() * m_plant.B(0, 0))) {
  if (plant.A(0, 0) == 0.0) {
    throw std::domain_error(
        "plant must have nonzero velocity damping A(0, 0); a plant built with "
        "kV = 0 doesn't determine the gearing.");
  }
  if (plant.B(0, 0) == 0.0) {
    throw std::domain_error("plant must have nonzero input gain B(0, 0).");
  }
}

void FlywheelSim::SetVelocity(wpi::units::radians_per_second_t velocity) {
  LinearSystemSim::SetState(wpi::math::Vectord<1>{velocity.value()});
}

wpi::units::radians_per_second_t FlywheelSim::GetAngularVelocity() const {
  return wpi::units::radians_per_second_t{GetOutput(0)};
}

wpi::units::radians_per_second_squared_t FlywheelSim::GetAngularAcceleration()
    const {
  return wpi::units::radians_per_second_squared_t{
      (m_plant.A() * m_x + m_plant.B() * m_u)(0, 0)};
}

wpi::units::newton_meter_t FlywheelSim::GetTorque() const {
  return wpi::units::newton_meter_t{GetAngularAcceleration().value() *
                                    m_j.value()};
}

wpi::units::ampere_t FlywheelSim::GetCurrentDraw() const {
  // Reductions are greater than 1, so a reduction of 10:1 would mean the motor
  // is spinning 10x faster than the output.
  //
  // The current through the motor is I = V/R - ω/(KᵥR), where V is the voltage
  // across the motor terminals.
  //
  // The motor controller produces V by PWMing the battery voltage at duty cycle
  // D = V/V_batt. An ideal H-bridge conserves power, so V_batt·I_supply = V·I,
  // giving
  //
  //   I_supply = D·I
  //
  // Scaling by D also makes the result continuous through V = 0, where the
  // motor is braking and its circulating current isn't drawn from the battery.
  wpi::units::volt_t appliedVoltage{m_u(0)};
  wpi::units::volt_t batteryVoltage = wpi::RobotController::GetBatteryVoltage();

  // With no battery voltage the controller can't apply any duty cycle, so
  // nothing is drawn.
  if (batteryVoltage == 0_V) {
    return 0_A;
  }

  double ratio = appliedVoltage.value() / batteryVoltage.value();
  double dutyCycle = std::clamp(ratio, -1.0, 1.0);
  return m_gearbox.Current(wpi::units::radians_per_second_t{m_x(0)} * m_gearing,
                           appliedVoltage) *
         dutyCycle;
}

wpi::units::volt_t FlywheelSim::GetInputVoltage() const {
  return wpi::units::volt_t{GetInput(0)};
}

void FlywheelSim::SetInputVoltage(wpi::units::volt_t voltage) {
  SetInput(wpi::math::Vectord<1>{voltage.value()});
  ClampInput(wpi::RobotController::GetBatteryVoltage().value());
}
