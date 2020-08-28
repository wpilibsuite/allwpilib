/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/simulation/FlywheelSim.h"

#include <wpi/MathExtras.h>

#include "frc/system/plant/LinearSystemId.h"

using namespace frc;
using namespace frc::sim;

FlywheelSim::FlywheelSim(const LinearSystem<1, 1, 1>& plant,
                         const DCMotor& gearbox, double gearing, bool addNoise,
                         const std::array<double, 1>& measurementStdDevs)
    : LinearSystemSim<1, 1, 1>(plant, addNoise, measurementStdDevs),
      m_motor(gearbox),
      m_gearing(gearing) {}

FlywheelSim::FlywheelSim(const DCMotor& gearbox, double gearing,
                         units::kilogram_square_meter_t moi, bool addNoise,
                         const std::array<double, 1>& measurementStdDevs)
    : FlywheelSim(LinearSystemId::FlywheelSystem(gearbox, moi, gearing),
                  gearbox, gearing, addNoise, measurementStdDevs) {}

units::radians_per_second_t FlywheelSim::GetAngularVelocity() const {
  return units::radians_per_second_t(Y()(0, 0));
}

units::ampere_t FlywheelSim::GetCurrentDraw() const {
  // I = V / R - omega / (Kv * R)
  // Reductions are greater than 1, so a reduction of 10:1 would mean the motor
  // is spinning 10x faster than the output.
  return m_motor.Current(GetAngularVelocity() * m_gearing,
                         units::volt_t(m_u(0))) *
         wpi::sgn(m_u(0));
}
