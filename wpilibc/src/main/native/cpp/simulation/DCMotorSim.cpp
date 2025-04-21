// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/DCMotorSim.h"

#include <wpi/MathExtras.h>

#include "frc/RobotController.h"

using namespace frc;
using namespace frc::sim;

DCMotorSim::DCMotorSim(const LinearSystem<2, 1, 2>& plant,
                       const DCMotor& gearbox,
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
      m_gearing(-gearbox.Kv.value() * m_plant.A(1, 1) / m_plant.B(1, 0)),
      m_j(m_gearing * gearbox.Kt.value() /
          (gearbox.R.value() * m_plant.B(1, 0))) {}

void DCMotorSim::SetState(units::radian_t angularPosition,
                          units::radians_per_second_t angularVelocity) {
  SetState(Vectord<2>{angularPosition, angularVelocity});
}

void DCMotorSim::SetAngle(units::radian_t angularPosition) {
  SetState(angularPosition, GetAngularVelocity());
}

void DCMotorSim::SetAngularVelocity(
    units::radians_per_second_t angularVelocity) {
  SetState(GetAngularPosition(), angularVelocity);
}

units::radian_t DCMotorSim::GetAngularPosition() const {
  return units::radian_t{GetOutput(0)};
}

units::radians_per_second_t DCMotorSim::GetAngularVelocity() const {
  return units::radians_per_second_t{GetOutput(1)};
}

units::radians_per_second_squared_t DCMotorSim::GetAngularAcceleration() const {
  return units::radians_per_second_squared_t{
      (m_plant.A() * m_x + m_plant.B() * m_u)(1, 0)};
}

units::newton_meter_t DCMotorSim::GetTorque() const {
  return units::newton_meter_t{GetAngularAcceleration().value() * m_j.value()};
}

units::ampere_t DCMotorSim::GetCurrentDraw() const {
  // I = V / R - omega / (Kv * R)
  // Reductions are greater than 1, so a reduction of 10:1 would mean the motor
  // is spinning 10x faster than the output.
  return m_gearbox.Current(units::radians_per_second_t{m_x(1)} * m_gearing,
                           units::volt_t{m_u(0)}) *
         wpi::sgn(m_u(0));
}

units::volt_t DCMotorSim::GetInputVoltage() const {
  return units::volt_t{GetInput(0)};
}

void DCMotorSim::SetInputVoltage(units::volt_t voltage) {
  SetInput(Vectord<1>{voltage.value()});
  ClampInput(frc::RobotController::GetBatteryVoltage().value());
}

const DCMotor& DCMotorSim::GetGearbox() const {
  return m_gearbox;
}

double DCMotorSim::GetGearing() const {
  return m_gearing;
}

units::kilogram_square_meter_t DCMotorSim::GetJ() const {
  return m_j;
}
