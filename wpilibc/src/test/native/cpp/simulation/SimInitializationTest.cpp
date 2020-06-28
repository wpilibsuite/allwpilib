/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <hal/HAL.h>

#include "frc/simulation/AccelerometerSim.h"
#include "frc/simulation/AnalogGyroSim.h"
#include "frc/simulation/AnalogInSim.h"
#include "frc/simulation/AnalogOutSim.h"
#include "frc/simulation/AnalogTriggerSim.h"
#include "frc/simulation/DIOSim.h"
#include "frc/simulation/DigitalPWMSim.h"
#include "frc/simulation/DriverStationSim.h"
#include "frc/simulation/EncoderSim.h"
#include "frc/simulation/PCMSim.h"
#include "frc/simulation/PDPSim.h"
#include "frc/simulation/PWMSim.h"
#include "frc/simulation/RelaySim.h"
#include "frc/simulation/RoboRioSim.h"
#include "frc/simulation/SPIAccelerometerSim.h"
#include "gtest/gtest.h"

using namespace frc::sim;

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
