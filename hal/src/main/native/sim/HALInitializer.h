/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>

namespace hal {
namespace init {
extern std::atomic_bool HAL_IsInitialized;
extern void RunInitialize();
static inline void CheckInit() {
  if (HAL_IsInitialized.load(std::memory_order_relaxed)) return;
  RunInitialize();
}

extern void InitializeAccelerometerData();
extern void InitializeAnalogGyroData();
extern void InitializeAnalogInData();
extern void InitializeAnalogOutData();
extern void InitializeAnalogTriggerData();
extern void InitializeCanData();
extern void InitializeCANAPI();
extern void InitializeDigitalPWMData();
extern void InitializeDIOData();
extern void InitializeDriverStationData();
extern void InitializeEncoderData();
extern void InitializeI2CData();
extern void InitializePCMData();
extern void InitializePDPData();
extern void InitializePWMData();
extern void InitializeRelayData();
extern void InitializeRoboRioData();
extern void InitializeSPIAccelerometerData();
extern void InitializeSPIData();
extern void InitializeAccelerometer();
extern void InitializeAnalogAccumulator();
extern void InitializeAnalogGyro();
extern void InitializeAnalogInput();
extern void InitializeAnalogInternal();
extern void InitializeAnalogOutput();
extern void InitializeCAN();
extern void InitializeCompressor();
extern void InitializeConstants();
extern void InitializeCounter();
extern void InitializeDigitalInternal();
extern void InitializeDIO();
extern void InitializeDriverStation();
extern void InitializeEncoder();
extern void InitializeExtensions();
extern void InitializeHAL();
extern void InitializeI2C();
extern void InitializeInterrupts();
extern void InitializeMockHooks();
extern void InitializeNotifier();
extern void InitializePDP();
extern void InitializePorts();
extern void InitializePower();
extern void InitializePWM();
extern void InitializeRelay();
extern void InitializeSerialPort();
extern void InitializeSolenoid();
extern void InitializeSPI();
extern void InitializeThreads();

}  // namespace init
}  // namespace hal
