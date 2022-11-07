// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/FlywheelSim.h"

#include <wpi/MathExtras.h>

#include "frc/system/plant/LinearSystemId.h"

using namespace frc;
using namespace frc::sim;

FlywheelSim::FlywheelSim(const LinearSystem<1, 1, 1>& plant,
                         const DCMotor& gearbox, double gearing,
                         const std::array<double, 1>& measurementStdDevs)
    : LinearSystemSim<1, 1, 1>(plant, measurementStdDevs),
      m_gearbox(gearbox),
      m_gearing(gearing) {}

FlywheelSim::FlywheelSim(const DCMotor& gearbox, double gearing,
                         units::kilogram_square_meter_t moi,
                         const std::array<double, 1>& measurementStdDevs)
    : FlywheelSim(LinearSystemId::FlywheelSystem(gearbox, moi, gearing),
                  gearbox, gearing, measurementStdDevs) {}

units::radians_per_second_t FlywheelSim::GetAngularVelocity() const {
  return units::radians_per_second_t{GetOutput(0)};
}

units::ampere_t FlywheelSim::GetCurrentDraw() const {
  // I = V / R - omega / (Kv * R)
  // Reductions are greater than 1, so a reduction of 10:1 would mean the motor
  // is spinning 10x faster than the output.
  return m_gearbox.Current(GetAngularVelocity() * m_gearing,
                           units::volt_t{m_u(0)}) *
         wpi::sgn(m_u(0));
}

void FlywheelSim::SetInputVoltage(units::volt_t voltage) {
  SetInput(Vectord<1>{voltage.value()});
}
