// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/FlywheelSim.h"

#include <wpi/MathExtras.h>

#include "frc/system/plant/LinearSystemId.h"

using namespace frc;
using namespace frc::sim;

// Some derivations
// a = A x + B u
// and
// a = ((-G² * K_t) / (K_v * R * J)) x + ((G * K_t) / (R * J)) u
// A = (-G² * K_t) / (K_v * R * J)
// B = (G * K_t) / (R * J)
// so J = (G K_t) / (R B)
// A / B = (-G) / K_v
// G = - A * K_v / B
FlywheelSim::FlywheelSim(const LinearSystem<1, 1, 1>& plant,
                         const DCMotor& gearbox,
                         const std::array<double, 1>& measurementStdDevs)
    : LinearSystemSim<1, 1, 1>(plant, measurementStdDevs),
      m_gearbox(gearbox),
      m_gearing(-gearbox.Kv.value() * m_plant.A(0, 0) / m_plant.B(0, 0)),
      m_j(m_gearing * gearbox.Kt.value() /
          (gearbox.R.value() * m_plant.B(0, 0))) {}

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
  // I = V / R - omega / (Kv * R)
  // Reductions are greater than 1, so a reduction of 10:1 would mean the motor
  // is spinning 10x faster than the output.
  return m_gearbox.Current(units::radians_per_second_t{m_x(0)} * m_gearing,
                           units::volt_t{m_u(0)}) *
         wpi::sgn(m_u(0));
}

units::volt_t FlywheelSim::GetInputVoltage() const {
  return units::volt_t{GetInput(0)};
}

void FlywheelSim::SetInputVoltage(units::volt_t voltage) {
  SetInput(Vectord<1>{voltage.value()});
  ClampInput(frc::RobotController::GetBatteryVoltage().value());
}
