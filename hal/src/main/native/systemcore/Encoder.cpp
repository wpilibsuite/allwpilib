// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Encoder.h"

#include <memory>

#include <fmt/format.h>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/Counter.h"
#include "hal/Errors.h"
#include "hal/handles/LimitedClassedHandleResource.h"

using namespace hal;

namespace hal {

namespace init {
void InitializeEncoder() {}
}  // namespace init
}  // namespace hal

extern "C" {
HAL_EncoderHandle HAL_InitializeEncoder(int32_t aChannel, int32_t bChannel,
                                        HAL_Bool reverseDirection,
                                        HAL_EncoderEncodingType encodingType,
                                        int32_t* status) {
  hal::init::CheckInit();
  *status = HAL_HANDLE_ERROR;
  return HAL_kInvalidHandle;
}

void HAL_FreeEncoder(HAL_EncoderHandle encoderHandle) {}

void HAL_SetEncoderSimDevice(HAL_EncoderHandle handle,
                             HAL_SimDeviceHandle device) {}

int32_t HAL_GetEncoder(HAL_EncoderHandle encoderHandle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int32_t HAL_GetEncoderRaw(HAL_EncoderHandle encoderHandle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int32_t HAL_GetEncoderEncodingScale(HAL_EncoderHandle encoderHandle,
                                    int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

void HAL_ResetEncoder(HAL_EncoderHandle encoderHandle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

double HAL_GetEncoderPeriod(HAL_EncoderHandle encoderHandle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

void HAL_SetEncoderMaxPeriod(HAL_EncoderHandle encoderHandle, double maxPeriod,
                             int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

HAL_Bool HAL_GetEncoderStopped(HAL_EncoderHandle encoderHandle,
                               int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return false;
}

HAL_Bool HAL_GetEncoderDirection(HAL_EncoderHandle encoderHandle,
                                 int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return false;
}

double HAL_GetEncoderDistance(HAL_EncoderHandle encoderHandle,
                              int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

double HAL_GetEncoderRate(HAL_EncoderHandle encoderHandle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

void HAL_SetEncoderMinRate(HAL_EncoderHandle encoderHandle, double minRate,
                           int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetEncoderDistancePerPulse(HAL_EncoderHandle encoderHandle,
                                    double distancePerPulse, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetEncoderReverseDirection(HAL_EncoderHandle encoderHandle,
                                    HAL_Bool reverseDirection,
                                    int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetEncoderSamplesToAverage(HAL_EncoderHandle encoderHandle,
                                    int32_t samplesToAverage, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

int32_t HAL_GetEncoderSamplesToAverage(HAL_EncoderHandle encoderHandle,
                                       int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

double HAL_GetEncoderDecodingScaleFactor(HAL_EncoderHandle encoderHandle,
                                         int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

double HAL_GetEncoderDistancePerPulse(HAL_EncoderHandle encoderHandle,
                                      int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

HAL_EncoderEncodingType HAL_GetEncoderEncodingType(
    HAL_EncoderHandle encoderHandle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return HAL_Encoder_k4X;
}

int32_t HAL_GetEncoderFPGAIndex(HAL_EncoderHandle encoderHandle,
                                int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

}  // extern "C"
