// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hal/Types.h"
#include "wpi/hal/simulation/NotifyListener.h"

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_SetIMUAngleX(double angle);
void HALSIM_SetIMUAngleY(double angle);
void HALSIM_SetIMUAngleZ(double angle);

void HALSIM_SetIMUGyroRateX(double rate);
void HALSIM_SetIMUGyroRateY(double rate);
void HALSIM_SetIMUGyroRateZ(double rate);

void HALSIM_SetIMUAccelX(double accel);
void HALSIM_SetIMUAccelY(double accel);
void HALSIM_SetIMUAccelZ(double accel);

void HALSIM_SetIMUYaw(double angle);

#ifdef __cplusplus
}  // extern "C"
#endif
