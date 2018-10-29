/*----------------------------------------------------------------------------*/
/* Copyright (c) 2013-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#ifndef HAL_USE_LABVIEW

#include "hal/Accelerometer.h"
#include "hal/AnalogAccumulator.h"
#include "hal/AnalogGyro.h"
#include "hal/AnalogInput.h"
#include "hal/AnalogOutput.h"
#include "hal/AnalogTrigger.h"
#include "hal/CAN.h"
#include "hal/Compressor.h"
#include "hal/Constants.h"
#include "hal/Counter.h"
#include "hal/DIO.h"
#include "hal/DriverStation.h"
#include "hal/Errors.h"
#include "hal/I2C.h"
#include "hal/Interrupts.h"
#include "hal/Notifier.h"
#include "hal/PDP.h"
#include "hal/PWM.h"
#include "hal/Ports.h"
#include "hal/Power.h"
#include "hal/Relay.h"
#include "hal/SPI.h"
#include "hal/SerialPort.h"
#include "hal/Solenoid.h"

#endif  // HAL_USE_LABVIEW

#include "hal/Types.h"
#include "hal/HALBase.h"

#ifdef __cplusplus
#include "hal/FRCUsageReporting.h"
#endif
