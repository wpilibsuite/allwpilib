// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <exception>

#include <gtest/gtest.h>

#include "wpi/hal/HAL.h"
#include "wpi/simulation/AddressableLEDSim.hpp"
#include "wpi/simulation/AnalogInputSim.hpp"
#include "wpi/simulation/CTREPCMSim.hpp"
#include "wpi/simulation/DIOSim.hpp"
#include "wpi/simulation/DigitalPWMSim.hpp"
#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/DutyCycleSim.hpp"
#include "wpi/simulation/EncoderSim.hpp"
#include "wpi/simulation/PWMSim.hpp"
#include "wpi/simulation/PowerDistributionSim.hpp"
#include "wpi/simulation/RoboRioSim.hpp"

using namespace frc::sim;

TEST(SimInitializationTest, AllInitialize) {
  HAL_Initialize(500, 0);
  AnalogInputSim aisim{0};
  EXPECT_THROW(DigitalPWMSim::CreateForChannel(0), std::out_of_range);
  DIOSim diosim{0};
  DriverStationSim dssim;
  (void)dssim;
  EncoderSim esim = EncoderSim::CreateForIndex(0);
  (void)esim;
  CTREPCMSim pcmsim{0};
  PowerDistributionSim pdpsim{0};
  PWMSim pwmsim{0};
  RoboRioSim rrsim;
  (void)rrsim;
  DutyCycleSim dcsim = DutyCycleSim::CreateForChannel(0);
  (void)dcsim;
  AddressableLEDSim adLED{0};
}
