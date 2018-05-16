/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/HAL.h"
#include "Simulation/AccelerometerSim.h"
#include "Simulation/AnalogGyroSim.h"
#include "Simulation/AnalogInSim.h"
#include "Simulation/AnalogOutSim.h"
#include "Simulation/AnalogTriggerSim.h"
#include "Simulation/DIOSim.h"
#include "Simulation/DigitalPWMSim.h"
#include "Simulation/DriverStationSim.h"
#include "Simulation/EncoderSim.h"
#include "Simulation/PCMSim.h"
#include "Simulation/PDPSim.h"
#include "Simulation/PWMSim.h"
#include "Simulation/RelaySim.h"
#include "Simulation/RoboRioSim.h"
#include "Simulation/SPIAccelerometerSim.h"
#include "gtest/gtest.h"

using namespace frc::sim;

namespace hal {

TEST(SimInitializationTests, TestAllInitialize) {
  HAL_Initialize(500, 0);
  AccelerometerSim acsim{0};
  AnalogGyroSim agsim{0};
  AnalogInSim aisim{0};
  AnalogOutSim aosim{0};
  AnalogTriggerSim atsim{0};
  DigitalPWMSim dpsim{0};
  DIOSim diosim{0};
  DriverStationSim dssim;
  (void)dssim;
  EncoderSim esim{0};
  PCMSim pcmsim{0};
  PDPSim pdpsim{0};
  PWMSim pwmsim{0};
  RelaySim rsim{0};
  RoboRioSim rrsim{0};
  SPIAccelerometerSim sasim{0};
}
}  // namespace hal
