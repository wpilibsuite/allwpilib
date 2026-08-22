// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/DCMotorSim.hpp"

#include "wpi/system/RobotController.hpp"
#include "wpi/util/MathExtras.hpp"

using namespace wpi;
using namespace wpi::sim;

DCMotorSim::DCMotorSim(const wpi::math::LinearSystem<2, 1, 2>& plant,
                       const wpi::math::DCMotor& gearbox,
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

void DCMotorSim::SetState(wpi::units::radians<> angularPosition,
                          wpi::units::radians_per_second<> angularVelocity) {
  SetState(wpi::math::Vectord<2>{angularPosition, angularVelocity});
}

void DCMotorSim::SetAngle(wpi::units::radians<> angularPosition) {
  SetState(angularPosition, GetAngularVelocity());
}

void DCMotorSim::SetAngularVelocity(
    wpi::units::radians_per_second<> angularVelocity) {
  SetState(GetAngularPosition(), angularVelocity);
}

wpi::units::radians<> DCMotorSim::GetAngularPosition() const {
  return wpi::units::radians<>{GetOutput(0)};
}

wpi::units::radians_per_second<> DCMotorSim::GetAngularVelocity() const {
  return wpi::units::radians_per_second<>{GetOutput(1)};
}

wpi::units::radians_per_second_squared<> DCMotorSim::GetAngularAcceleration()
    const {
  return wpi::units::radians_per_second_squared<>{
      (m_plant.A() * m_x + m_plant.B() * m_u)(1, 0)};
}

wpi::units::newton_meters<> DCMotorSim::GetTorque() const {
  return wpi::units::newton_meters<>{GetAngularAcceleration().value() *
                                     m_j.value()};
}

wpi::units::amperes<> DCMotorSim::GetCurrentDraw() const {
  // I = V / R - omega / (Kv * R)
  // Reductions are greater than 1, so a reduction of 10:1 would mean the motor
  // is spinning 10x faster than the output.
  return m_gearbox.Current(wpi::units::radians_per_second<>{m_x(1)} * m_gearing,
                           wpi::units::volts<>{m_u(0)}) *
         wpi::util::sgn(m_u(0));
}

wpi::units::volts<> DCMotorSim::GetInputVoltage() const {
  return wpi::units::volts<>{GetInput(0)};
}

void DCMotorSim::SetInputVoltage(wpi::units::volts<> voltage) {
  SetInput(wpi::math::Vectord<1>{voltage.value()});
  ClampInput(wpi::RobotController::GetBatteryVoltage().value());
}

const wpi::math::DCMotor& DCMotorSim::GetGearbox() const {
  return m_gearbox;
}

double DCMotorSim::GetGearing() const {
  return m_gearing;
}

wpi::units::kilogram_square_meters<> DCMotorSim::GetJ() const {
  return m_j;
}
