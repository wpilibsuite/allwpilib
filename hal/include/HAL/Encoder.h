/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/AnalogTrigger.h"
#include "HAL/Handles.h"

extern "C" {
enum EncoderIndexingType {
  HAL_kResetWhileHigh,
  HAL_kResetWhileLow,
  HAL_kResetOnFallingEdge,
  HAL_kResetOnRisingEdge
};
enum EncoderEncodingType { HAL_Encoder_k1X, HAL_Encoder_k2X, HAL_Encoder_k4X };

HalEncoderHandle initializeEncoder(
    HalHandle digitalSourceHandleA, AnalogTriggerType analogTriggerTypeA,
    HalHandle digitalSourceHandleB, AnalogTriggerType analogTriggerTypeB,
    bool reverseDirection, EncoderEncodingType encodingType, int32_t* status);
void freeEncoder(HalEncoderHandle encoder_handle, int32_t* status);
int32_t getEncoder(HalEncoderHandle encoder_handle, int32_t* status);
int32_t getEncoderRaw(HalEncoderHandle encoder_handle, int32_t* status);
int32_t getEncoderEncodingScale(HalEncoderHandle encoder_handle,
                                int32_t* status);
void resetEncoder(HalEncoderHandle encoder_handle, int32_t* status);
int32_t getEncoderPeriod(HalEncoderHandle encoder_handle, int32_t* status);
void setEncoderMaxPeriod(HalEncoderHandle encoder_handle, double maxPeriod,
                         int32_t* status);
uint8_t getEncoderStopped(HalEncoderHandle encoder_handle, int32_t* status);
uint8_t getEncoderDirection(HalEncoderHandle encoder_handle, int32_t* status);
double getEncoderDistance(HalEncoderHandle encoder_handle, int32_t* status);
double getEncoderRate(HalEncoderHandle encoder_handle, int32_t* status);
void setEncoderMinRate(HalEncoderHandle encoder_handle, double minRate,
                       int32_t* status);
void setEncoderDistancePerPulse(HalEncoderHandle encoder_handle,
                                double distancePerPulse, int32_t* status);
void setEncoderReverseDirection(HalEncoderHandle encoder_handle,
                                uint8_t reverseDirection, int32_t* status);
void setEncoderSamplesToAverage(HalEncoderHandle encoder_handle,
                                int32_t samplesToAverage, int32_t* status);
int32_t getEncoderSamplesToAverage(HalEncoderHandle encoder_handle,
                                   int32_t* status);

void setEncoderIndexSource(HalEncoderHandle encoder_handle,
                           HalHandle digitalSourceHandle,
                           AnalogTriggerType analogTriggerType,
                           EncoderIndexingType type, int32_t* status);

int32_t getEncoderFPGAIndex(HalEncoderHandle encoder_handle, int32_t* status);

double getEncoderDecodingScaleFactor(HalEncoderHandle encoder_handle,
                                     int32_t* status);

double getEncoderDistancePerPulse(HalEncoderHandle encoder_handle,
                                  int32_t* status);

EncoderEncodingType getEncoderEncodingType(HalEncoderHandle encoder_handle,
                                           int32_t* status);
}
