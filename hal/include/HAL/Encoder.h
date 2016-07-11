/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "AnalogTrigger.h"
#include "Types.h"

extern "C" {
enum HAL_EncoderIndexingType {
  HAL_kResetWhileHigh,
  HAL_kResetWhileLow,
  HAL_kResetOnFallingEdge,
  HAL_kResetOnRisingEdge
};
enum HAL_EncoderEncodingType {
  HAL_Encoder_k1X,
  HAL_Encoder_k2X,
  HAL_Encoder_k4X
};

HAL_EncoderHandle HAL_InitializeEncoder(
    HAL_Handle digitalSourceHandleA, HAL_AnalogTriggerType analogTriggerTypeA,
    HAL_Handle digitalSourceHandleB, HAL_AnalogTriggerType analogTriggerTypeB,
    HAL_Bool reverseDirection, HAL_EncoderEncodingType encodingType,
    int32_t* status);
void HAL_FreeEncoder(HAL_EncoderHandle encoder_handle, int32_t* status);
int32_t HAL_GetEncoder(HAL_EncoderHandle encoder_handle, int32_t* status);
int32_t HAL_GetEncoderRaw(HAL_EncoderHandle encoder_handle, int32_t* status);
int32_t HAL_GetEncoderEncodingScale(HAL_EncoderHandle encoder_handle,
                                    int32_t* status);
void HAL_ResetEncoder(HAL_EncoderHandle encoder_handle, int32_t* status);
double HAL_GetEncoderPeriod(HAL_EncoderHandle encoder_handle, int32_t* status);
void HAL_SetEncoderMaxPeriod(HAL_EncoderHandle encoder_handle, double maxPeriod,
                             int32_t* status);
HAL_Bool HAL_GetEncoderStopped(HAL_EncoderHandle encoder_handle,
                               int32_t* status);
HAL_Bool HAL_GetEncoderDirection(HAL_EncoderHandle encoder_handle,
                                 int32_t* status);
double HAL_GetEncoderDistance(HAL_EncoderHandle encoder_handle,
                              int32_t* status);
double HAL_GetEncoderRate(HAL_EncoderHandle encoder_handle, int32_t* status);
void HAL_SetEncoderMinRate(HAL_EncoderHandle encoder_handle, double minRate,
                           int32_t* status);
void HAL_SetEncoderDistancePerPulse(HAL_EncoderHandle encoder_handle,
                                    double distancePerPulse, int32_t* status);
void HAL_SetEncoderReverseDirection(HAL_EncoderHandle encoder_handle,
                                    HAL_Bool reverseDirection, int32_t* status);
void HAL_SetEncoderSamplesToAverage(HAL_EncoderHandle encoder_handle,
                                    int32_t samplesToAverage, int32_t* status);
int32_t HAL_GetEncoderSamplesToAverage(HAL_EncoderHandle encoder_handle,
                                       int32_t* status);

void HAL_SetEncoderIndexSource(HAL_EncoderHandle encoder_handle,
                               HAL_Handle digitalSourceHandle,
                               HAL_AnalogTriggerType analogTriggerType,
                               HAL_EncoderIndexingType type, int32_t* status);

int32_t HAL_GetEncoderFPGAIndex(HAL_EncoderHandle encoder_handle,
                                int32_t* status);

double HAL_GetEncoderDecodingScaleFactor(HAL_EncoderHandle encoder_handle,
                                         int32_t* status);

double HAL_GetEncoderDistancePerPulse(HAL_EncoderHandle encoder_handle,
                                      int32_t* status);

HAL_EncoderEncodingType HAL_GetEncoderEncodingType(
    HAL_EncoderHandle encoder_handle, int32_t* status);
}
