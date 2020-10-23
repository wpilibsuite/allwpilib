/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/simulation/ADXRS450_GyroSim.h"

#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#include "frc/ADXRS450_Gyro.h"
#include "frc/simulation/SimDeviceSim.h"

using namespace frc::sim;

ADXRS450_GyroSim::ADXRS450_GyroSim(const frc::ADXRS450_Gyro& gyro) {
  wpi::SmallString<128> fullname;
  wpi::raw_svector_ostream os(fullname);
  os << "ADXRS450_Gyro" << '[' << gyro.GetPort() << ']';
  frc::sim::SimDeviceSim deviceSim{fullname.c_str()};
  m_simAngle = deviceSim.GetDouble("Angle");
  m_simRate = deviceSim.GetDouble("Rate");
}

void ADXRS450_GyroSim::SetAngle(units::degree_t angle) {
  m_simAngle.Set(angle.to<double>());
}

void ADXRS450_GyroSim::SetRate(units::degrees_per_second_t rate) {
  m_simRate.Set(rate.to<double>());
}
