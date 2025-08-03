// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/FlywheelSim.h"

#include <wpi/MathExtras.h>

#include "frc/RobotController.h"
#include "frc/system/NumericalIntegration.h"

using namespace frc;
using namespace frc::sim;

FlywheelSim::FlywheelSim(const units::volt_t ks,
                         const LinearSystem<1, 1, 1>& plant,
                         const DCMotor& gearbox,
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
          (gearbox.R.value() * m_plant.B(0, 0))),
      m_ks{ks},
      m_frictionAcceleration{ks.value() * m_gearing * gearbox.Kt.value() /
                             (gearbox.R.value() * m_j.value())} {}

void FlywheelSim::SetVelocity(units::radians_per_second_t velocity) {
  LinearSystemSim::SetState(Vectord<1>{velocity.value()});
}

units::radians_per_second_t FlywheelSim::GetAngularVelocity() const {
  return units::radians_per_second_t{GetOutput(0)};
}

units::radians_per_second_squared_t FlywheelSim::GetAngularAcceleration()
    const {
  return units::radians_per_second_squared_t{
      (m_plant.A() * m_x + m_plant.B() * m_u)(0, 0)};
}

units::newton_meter_t FlywheelSim::GetTorque() const {
  return units::newton_meter_t{GetAngularAcceleration().value() * m_j.value()};
}

units::ampere_t FlywheelSim::GetCurrentDraw() const {
  // V = IR + omega / Kv + ks * sgn(omega)
  // IR = V - omega / Kv - ks * sgn(omega)
  // I = V / R - omega / (Kv * R) - ks * sgn(omega) / R
  // I = (V - ks * sgn(omega)) / R - omega / (Kv * R)
  // Reductions are greater than 1, so a reduction of 10:1 would mean the motor
  // is spinning 10x faster than the output.
  units::volt_t frictionVoltage = wpi::sgn(m_x(0)) * m_ks;
  return m_gearbox.Current(units::radians_per_second_t{m_x(0)} * m_gearing,
                           units::volt_t{m_u(0) - frictionVoltage.value()}) *
         wpi::sgn(m_u(0) - frictionVoltage.value());
}

units::volt_t FlywheelSim::GetInputVoltage() const {
  return units::volt_t{GetInput(0)};
}

void FlywheelSim::SetInputVoltage(units::volt_t voltage) {
  SetInput(Vectord<1>{voltage.value()});
  ClampInput(frc::RobotController::GetBatteryVoltage().value());
}

Vectord<1> FlywheelSim::UpdateX(const Vectord<1>& currentXhat,
                                const Vectord<1>& u, units::second_t dt) {
  Vectord<1> updatedXhat = RKDP(
      [&](const auto& x, const auto& u) -> Vectord<1> {
        Vectord<1> xdot = m_plant.A() * x + m_plant.B() * u +
                          Vectord<1>{-m_frictionAcceleration.value()} * wpi::sgn(x(0));
        return xdot;
      },
      currentXhat, u, dt);

  return updatedXhat;
}
