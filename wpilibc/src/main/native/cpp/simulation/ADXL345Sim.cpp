// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/ADXL345Sim.h"

#include "frc/ADXL345_I2C.h"
#include "frc/ADXL345_SPI.h"
#include "frc/simulation/SimDeviceSim.h"

using namespace frc::sim;

ADXL345Sim::ADXL345Sim(const frc::ADXL345_I2C& accel) {
  frc::sim::SimDeviceSim deviceSim{"Accel:ADXL345_I2C", accel.GetI2CPort(),
                                   accel.GetI2CDeviceAddress()};
  m_simX = deviceSim.GetDouble("x");
  m_simY = deviceSim.GetDouble("y");
  m_simZ = deviceSim.GetDouble("z");
}

ADXL345Sim::ADXL345Sim(const frc::ADXL345_SPI& accel) {
  frc::sim::SimDeviceSim deviceSim{"Accel:ADXL345_SPI", accel.GetSpiPort()};
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
