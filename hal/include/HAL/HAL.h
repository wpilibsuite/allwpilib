/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2013-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "FRC_NetworkCommunication/UsageReporting.h"
#include "HAL/Accelerometer.h"
#include "HAL/AnalogAccumulator.h"
#include "HAL/AnalogGyro.h"
#include "HAL/AnalogInput.h"
#include "HAL/AnalogOutput.h"
#include "HAL/AnalogTrigger.h"
#include "HAL/Compressor.h"
#include "HAL/Constants.h"
#include "HAL/Counter.h"
#include "HAL/DIO.h"
#include "HAL/DriverStation.h"
#include "HAL/Errors.h"
#include "HAL/I2C.h"
#include "HAL/Interrupts.h"
#include "HAL/Notifier.h"
#include "HAL/PDP.h"
#include "HAL/PWM.h"
#include "HAL/Ports.h"
#include "HAL/Power.h"
#include "HAL/Relay.h"
#include "HAL/SPI.h"
#include "HAL/SerialPort.h"
#include "HAL/Solenoid.h"
#include "HAL/Task.h"
#include "HAL/Types.h"

namespace HALUsageReporting = nUsageReporting;

#ifdef __cplusplus
extern "C" {
#endif
HAL_PortHandle HAL_GetPort(int32_t channel);
HAL_PortHandle HAL_GetPortWithModule(int32_t module, int32_t channel);
const char* HAL_GetErrorMessage(int32_t code);

int32_t HAL_GetFPGAVersion(int32_t* status);
int64_t HAL_GetFPGARevision(int32_t* status);
uint64_t HAL_GetFPGATime(int32_t* status);

HAL_Bool HAL_GetFPGAButton(int32_t* status);

HAL_Bool HAL_GetSystemActive(int32_t* status);
HAL_Bool HAL_GetBrownedOut(int32_t* status);

int32_t HAL_Initialize(int32_t mode);

// ifdef's definition is to allow for default parameters in C++.
#ifdef __cplusplus
int64_t HAL_Report(int32_t resource, int32_t instanceNumber,
                   int32_t context = 0, const char* feature = nullptr);
#else
int64_t HAL_Report(int32_t resource, int32_t instanceNumber, int32_t context,
                   const char* feature);
#endif
#ifdef __cplusplus
}
#endif
