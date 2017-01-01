/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/AnalogTrigger.h"
#include "HAL/Types.h"

enum HAL_EncoderIndexingType : int32_t {
  HAL_kResetWhileHigh,
  HAL_kResetWhileLow,
  HAL_kResetOnFallingEdge,
  HAL_kResetOnRisingEdge
};
enum HAL_EncoderEncodingType : int32_t {
  HAL_Encoder_k1X,
  HAL_Encoder_k2X,
  HAL_Encoder_k4X
};

#ifdef __cplusplus
extern "C" {
#endif
HAL_EncoderHandle HAL_InitializeEncoder(
    HAL_Handle digitalSourceHandleA, HAL_AnalogTriggerType analogTriggerTypeA,
    HAL_Handle digitalSourceHandleB, HAL_AnalogTriggerType analogTriggerTypeB,
    HAL_Bool reverseDirection, HAL_EncoderEncodingType encodingType,
    int32_t* status);
void HAL_FreeEncoder(HAL_EncoderHandle encoderHandle, int32_t* status);
int32_t HAL_GetEncoder(HAL_EncoderHandle encoderHandle, int32_t* status);
int32_t HAL_GetEncoderRaw(HAL_EncoderHandle encoderHandle, int32_t* status);
int32_t HAL_GetEncoderEncodingScale(HAL_EncoderHandle encoderHandle,
                                    int32_t* status);
void HAL_ResetEncoder(HAL_EncoderHandle encoderHandle, int32_t* status);
double HAL_GetEncoderPeriod(HAL_EncoderHandle encoderHandle, int32_t* status);
void HAL_SetEncoderMaxPeriod(HAL_EncoderHandle encoderHandle, double maxPeriod,
                             int32_t* status);
HAL_Bool HAL_GetEncoderStopped(HAL_EncoderHandle encoderHandle,
                               int32_t* status);
HAL_Bool HAL_GetEncoderDirection(HAL_EncoderHandle encoderHandle,
                                 int32_t* status);
double HAL_GetEncoderDistance(HAL_EncoderHandle encoderHandle, int32_t* status);
double HAL_GetEncoderRate(HAL_EncoderHandle encoderHandle, int32_t* status);
void HAL_SetEncoderMinRate(HAL_EncoderHandle encoderHandle, double minRate,
                           int32_t* status);
void HAL_SetEncoderDistancePerPulse(HAL_EncoderHandle encoderHandle,
                                    double distancePerPulse, int32_t* status);
void HAL_SetEncoderReverseDirection(HAL_EncoderHandle encoderHandle,
                                    HAL_Bool reverseDirection, int32_t* status);
void HAL_SetEncoderSamplesToAverage(HAL_EncoderHandle encoderHandle,
                                    int32_t samplesToAverage, int32_t* status);
int32_t HAL_GetEncoderSamplesToAverage(HAL_EncoderHandle encoderHandle,
                                       int32_t* status);

void HAL_SetEncoderIndexSource(HAL_EncoderHandle encoderHandle,
                               HAL_Handle digitalSourceHandle,
                               HAL_AnalogTriggerType analogTriggerType,
                               HAL_EncoderIndexingType type, int32_t* status);

int32_t HAL_GetEncoderFPGAIndex(HAL_EncoderHandle encoderHandle,
                                int32_t* status);

double HAL_GetEncoderDecodingScaleFactor(HAL_EncoderHandle encoderHandle,
                                         int32_t* status);

double HAL_GetEncoderDistancePerPulse(HAL_EncoderHandle encoderHandle,
                                      int32_t* status);

HAL_EncoderEncodingType HAL_GetEncoderEncodingType(
    HAL_EncoderHandle encoderHandle, int32_t* status);
#ifdef __cplusplus
}
#endif
