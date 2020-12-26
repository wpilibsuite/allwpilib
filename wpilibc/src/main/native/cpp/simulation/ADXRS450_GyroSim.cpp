// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/ADXRS450_GyroSim.h"

#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#include "frc/ADXRS450_Gyro.h"
#include "frc/simulation/SimDeviceSim.h"

using namespace frc::sim;

ADXRS450_GyroSim::ADXRS450_GyroSim(const frc::ADXRS450_Gyro& gyro) {
  wpi::SmallString<128> fullname;
  wpi::raw_svector_ostream os(fullname);
  os << "Gyro:ADXRS450" << '[' << gyro.GetPort() << ']';
  frc::sim::SimDeviceSim deviceSim{fullname.c_str()};
  m_simAngle = deviceSim.GetDouble("angle_x");
  m_simRate = deviceSim.GetDouble("rate_x");
}

void ADXRS450_GyroSim::SetAngle(units::degree_t angle) {
  m_simAngle.Set(angle.to<double>());
}

void ADXRS450_GyroSim::SetRate(units::degrees_per_second_t rate) {
  m_simRate.Set(rate.to<double>());
}
