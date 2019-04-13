/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "gtest/gtest.h"
#include "hal/HAL.h"
#include "simulation/AccelerometerSim.h"
#include "simulation/AnalogGyroSim.h"
#include "simulation/AnalogInSim.h"
#include "simulation/AnalogOutSim.h"
#include "simulation/AnalogTriggerSim.h"
#include "simulation/DIOSim.h"
#include "simulation/DigitalPWMSim.h"
#include "simulation/DriverStationSim.h"
#include "simulation/EncoderSim.h"
#include "simulation/PCMSim.h"
#include "simulation/PDPSim.h"
#include "simulation/PWMSim.h"
#include "simulation/RelaySim.h"
#include "simulation/RoboRioSim.h"
#include "simulation/SPIAccelerometerSim.h"

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
