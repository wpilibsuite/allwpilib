/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/Handles.h"

extern "C" {
HalGyroHandle initializeAnalogGyro(HalAnalogInputHandle handle,
                                   int32_t* status);
void setupAnalogGyro(HalGyroHandle handle, int32_t* status);
void freeAnalogGyro(HalGyroHandle handle);
void setAnalogGyroParameters(HalGyroHandle handle,
                             float voltsPerDegreePerSecond, float offset,
                             uint32_t center, int32_t* status);
void setAnalogGyroVoltsPerDegreePerSecond(HalGyroHandle handle,
                                          float voltsPerDegreePerSecond,
                                          int32_t* status);
void resetAnalogGyro(HalGyroHandle handle, int32_t* status);
void calibrateAnalogGyro(HalGyroHandle handle, int32_t* status);
void setAnalogGyroDeadband(HalGyroHandle handle, float volts, int32_t* status);
float getAnalogGyroAngle(HalGyroHandle handle, int32_t* status);
double getAnalogGyroRate(HalGyroHandle handle, int32_t* status);
float getAnalogGyroOffset(HalGyroHandle handle, int32_t* status);
uint32_t getAnalogGyroCenter(HalGyroHandle handle, int32_t* status);
}
