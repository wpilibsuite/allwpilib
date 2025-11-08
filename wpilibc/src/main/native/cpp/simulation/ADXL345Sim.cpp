// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/ADXL345Sim.hpp"

#include "wpi/hardware/accelerometer/ADXL345_I2C.hpp"
#include "wpi/simulation/SimDeviceSim.hpp"

using namespace frc::sim;

ADXL345Sim::ADXL345Sim(const frc::ADXL345_I2C& accel) {
  frc::sim::SimDeviceSim deviceSim{"Accel:ADXL345_I2C", accel.GetI2CPort(),
                                   accel.GetI2CDeviceAddress()};
  m_simX = deviceSim.GetDouble("x");
  m_simY = deviceSim.GetDouble("y");
  m_simZ = deviceSim.GetDouble("z");
}

void ADXL345Sim::SetX(double accel) {
  m_simX.Set(accel);
}

void ADXL345Sim::SetY(double accel) {
  m_simY.Set(accel);
}

void ADXL345Sim::SetZ(double accel) {
  m_simZ.Set(accel);
}
