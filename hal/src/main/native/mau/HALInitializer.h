/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>
#include <AHRS.h>
#include <VMXTime.h>
#include <VMXIO.h>
#include <VMXCAN.h>
#include <VMXPower.h>
#include <VMXVersion.h>
#include <VMXThread.h>

namespace hal {
    namespace init {
        extern std::atomic_bool HAL_IsInitialized;

        extern void RunInitialize();

        static inline void CheckInit() {
            if (HAL_IsInitialized.load(std::memory_order_relaxed)) return;
            RunInitialize();
        }

        extern void InitializeAccelerometer(AHRS* ahrs);
        extern void InitializeAnalogAccumulator(VMXIO* io);
        extern void InitializeAnalogGyro(AHRS* ahrs);
        extern void InitializeAnalogInput(VMXIO* io);
        extern void InitializeAnalogInternal();
        extern void InitializeAnalogOutput();  //DNE
        extern void InitializeCAN(VMXCAN* can);
        extern void InitializeCompressor(VMXCAN* can);
        extern void InitializeConstants();
        extern void InitializeCounter(VMXTime* time);
        extern void InitializeDigitalInternal(VMXIO* io);
        extern void InitializeDIO(VMXIO* io);
        extern void InitializeDriverStation();  //DNE
        extern void InitializeEncoder(VMXIO* io);
        extern void InitializeExtensions();
        extern void InitializeHAL();
        extern void InitializeI2C(VMXIO* io);
        extern void InitializeInterrupts(VMXIO* io);
        extern void InitializeMockHooks();
        extern void InitializeNotifier(VMXTime* time);
        extern void InitializeOSSerialPort(VMXCAN* can);
        extern void InitializePDP(VMXCAN* can);
        extern void InitializePorts();
        extern void InitializePower(VMXPower* power);
        extern void InitializePWM(VMXIO* io);
        extern void InitializeRelay(VMXIO* io);
        extern void InitializeSerialPort(AHRS* ahrs);
        extern void InitializeSolenoid();
        extern void InitializeSPI(VMXIO* io);
        extern void InitializeThreads(VMXThread* thread);

    }  // namespace init
}  // namespace hal
