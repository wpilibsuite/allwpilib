// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/AngularMechanismSim.h"

#include <wpi/MathExtras.h>

#include "frc/system/plant/LinearSystemId.h"

using namespace frc;
using namespace frc::sim;

AngularMechanismSim::AngularMechanismSim(
    const LinearSystem<2, 1, 2>& plant, const DCMotor& gearbox,
    const std::array<double, 2>& measurementStdDevs)
    : LinearSystemSim<2, 1, 2>(plant, measurementStdDevs),
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
      m_gearing(-gearbox.Kv.value() * plant.A(1, 1) / plant.B(1, 0)),
      m_j(units::kilogram_square_meter_t{
          m_gearing * m_gearbox.Kt.value() /
          (m_gearbox.R.value() * m_plant.B(0, 0))}) {}

units::kilogram_square_meter_t AngularMechanismSim::GetJ() const {
  return m_j;
}

void AngularMechanismSim::SetState(
    units::radian_t angularPosition,
    units::radians_per_second_t angularVelocity) {
  SetState(Vectord<2>{angularPosition, angularVelocity});
}

void AngularMechanismSim::SetPosition(units::radian_t angularPosition) {
  SetState(Vectord<2>{angularPosition, m_x(1, 0)});
}

void AngularMechanismSim::SetVelocity(
    units::radians_per_second_t angularVelocity) {
  SetState(Vectord<2>{m_x(0, 0), angularVelocity});
}

units::radian_t AngularMechanismSim::GetAngularPosition() const {
  return units::radian_t{GetOutput(0)};
}

units::radians_per_second_t AngularMechanismSim::GetAngularVelocity() const {
  return units::radians_per_second_t{GetOutput(1)};
}

units::radians_per_second_squared_t
AngularMechanismSim::GetAngularAcceleration() const {
  return units::radians_per_second_squared_t{
      (m_plant.A() * m_x + m_plant.B() * m_u)(0, 0)};
}

units::newton_meter_t AngularMechanismSim::GetTorque() const {
  return units::newton_meter_t{GetJ().value() *
                               GetAngularAcceleration().value()};
}

units::ampere_t AngularMechanismSim::GetCurrentDraw() const {
  // I = V / R - omega / (Kv * R)
  // Reductions are greater than 1, so a reduction of 10:1 would mean the motor
  // is spinning 10x faster than the output.
  return m_gearbox.Current(units::radians_per_second_t{m_x(1)} * m_gearing,
                           units::volt_t{m_u(0)}) *
         wpi::sgn(m_u(0));
}

void AngularMechanismSim::SetInputVoltage(units::volt_t voltage) {
  SetInput(Vectord<1>{voltage.value()});
  ClampInput(frc::RobotController::GetBatteryVoltage().value());
}

units::volt_t AngularMechanismSim::GetInputVoltage() const {
  return units::volt_t{GetInput(0)};
}
