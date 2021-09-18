// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <exception>

#include <hal/HAL.h>

#include "frc/simulation/AddressableLEDSim.h"
#include "frc/simulation/AnalogGyroSim.h"
#include "frc/simulation/AnalogInputSim.h"
#include "frc/simulation/AnalogOutputSim.h"
#include "frc/simulation/AnalogTriggerSim.h"
#include "frc/simulation/BuiltInAccelerometerSim.h"
#include "frc/simulation/CTREPCMSim.h"
#include "frc/simulation/DIOSim.h"
#include "frc/simulation/DigitalPWMSim.h"
#include "frc/simulation/DriverStationSim.h"
#include "frc/simulation/DutyCycleSim.h"
#include "frc/simulation/EncoderSim.h"
#include "frc/simulation/PWMSim.h"
#include "frc/simulation/PowerDistributionSim.h"
#include "frc/simulation/RelaySim.h"
#include "frc/simulation/RoboRioSim.h"
#include "frc/simulation/SPIAccelerometerSim.h"
#include "gtest/gtest.h"

using namespace frc::sim;

TEST(SimInitializationTest, AllInitialize) {
  HAL_Initialize(500, 0);
  BuiltInAccelerometerSim biacsim;
  AnalogGyroSim agsim{0};
  AnalogInputSim aisim{0};
  AnalogOutputSim aosim{0};
  EXPECT_THROW(AnalogTriggerSim::CreateForChannel(0), std::out_of_range);
  EXPECT_THROW(DigitalPWMSim::CreateForChannel(0), std::out_of_range);
  DIOSim diosim{0};
  DriverStationSim dssim;
  (void)dssim;
  EncoderSim esim = EncoderSim::CreateForIndex(0);
  (void)esim;
  CTREPCMSim pcmsim{0};
  PowerDistributionSim pdpsim{0};
  PWMSim pwmsim{0};
  RelaySim rsim{0};
  RoboRioSim rrsim;
  (void)rrsim;
  SPIAccelerometerSim sasim{0};
  DutyCycleSim dcsim = DutyCycleSim::CreateForIndex(0);
  (void)dcsim;
  AddressableLEDSim adLED;
}
