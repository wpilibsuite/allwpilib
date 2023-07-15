// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/DCMotorSim.h"

#include <wpi/MathExtras.h>

#include "frc/system/plant/LinearSystemId.h"

using namespace frc;
using namespace frc::sim;

DCMotorSim::DCMotorSim(const LinearSystem<2, 1, 2>& plant,
                       const DCMotor& gearbox, double gearing,
                       const std::array<double, 2>& measurementStdDevs)
    : LinearSystemSim<2, 1, 2>(plant, measurementStdDevs),
      m_gearbox(gearbox),
      m_gearing(gearing) {}

DCMotorSim::DCMotorSim(const DCMotor& gearbox, double gearing,
                       units::kilogram_square_meter_t moi,
                       const std::array<double, 2>& measurementStdDevs)
    : DCMotorSim(LinearSystemId::DCMotorSystem(gearbox, moi, gearing), gearbox,
                 gearing, measurementStdDevs) {}

units::radian_t DCMotorSim::GetAngularPosition() const {
  return units::radian_t{GetOutput(0)};
}

units::radians_per_second_t DCMotorSim::GetAngularVelocity() const {
  return units::radians_per_second_t{GetOutput(1)};
}

units::ampere_t DCMotorSim::GetCurrentDraw() const {
  // I = V / R - omega / (Kv * R)
  // Reductions are greater than 1, so a reduction of 10:1 would mean the motor
  // is spinning 10x faster than the output.
  return m_gearbox.Current(GetAngularVelocity() * m_gearing,
                           units::volt_t{m_u(0)}) *
         wpi::sgn(m_u(0));
}

void DCMotorSim::SetInputVoltage(units::volt_t voltage) {
  SetInput(Vectord<1>{voltage.value()});
}
