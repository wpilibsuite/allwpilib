// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/ADXL362Sim.h"

#include "frc/ADXL362.h"
#include "frc/simulation/SimDeviceSim.h"

using namespace frc::sim;

ADXL362Sim::ADXL362Sim(const frc::ADXL362& accel) {
  frc::sim::SimDeviceSim deviceSim{"Accel:ADXL362", accel.GetSpiPort()};
  m_simX = deviceSim.GetDouble("x");
  m_simY = deviceSim.GetDouble("y");
  m_simZ = deviceSim.GetDouble("z");
}

void ADXL362Sim::SetX(double accel) {
  m_simX.Set(accel);
}

void ADXL362Sim::SetY(double accel) {
  m_simY.Set(accel);
}

void ADXL362Sim::SetZ(double accel) {
  m_simZ.Set(accel);
}
