// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>

namespace hal::init {
extern std::atomic_bool HAL_IsInitialized;
extern void RunInitialize();
inline void CheckInit() {
  if (HAL_IsInitialized.load(std::memory_order_relaxed)) {
    return;
  }
  RunInitialize();
}

extern void InitializeAddressableLEDData();
extern void InitializeAnalogInData();
extern void InitializeCanData();
extern void InitializeCANAPI();
extern void InitializeDigitalPWMData();
extern void InitializeDutyCycleData();
extern void InitializeDIOData();
extern void InitializeDutyCycle();
extern void InitializeDriverStationData();
extern void InitializeEncoderData();
extern void InitializeI2CData();
extern void InitializeCTREPCMData();
extern void InitializeREVPHData();
extern void InitializePowerDistributionData();
extern void InitializePWMData();
extern void InitializeRoboRioData();
extern void InitializeSimDeviceData();
extern void InitializeAddressableLED();
extern void InitializeAnalogInput();
extern void InitializeAnalogInternal();
extern void InitializeCAN();
extern void InitializeConstants();
extern void InitializeCounter();
extern void InitializeDigitalInternal();
extern void InitializeDIO();
extern void InitializeDriverStation();
extern void InitializeEncoder();
extern void InitializeExtensions();
extern void InitializeHAL();
extern void InitializeI2C();
extern void InitializeMain();
extern void InitializeMockHooks();
extern void InitializeNotifier();
extern void InitializePowerDistribution();
extern void InitializePorts();
extern void InitializePower();
extern void InitializeCTREPCM();
extern void InitializeREVPH();
extern void InitializePWM();
extern void InitializeSerialPort();
extern void InitializeSimDevice();
extern void InitializeThreads();

}  // namespace hal::init
