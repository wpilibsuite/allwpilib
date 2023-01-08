// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_rev_ph REV PH Functions
 * @ingroup hal_capi
 * @{
 */

/**
 * The compressor configuration type
 */
HAL_ENUM(HAL_REVPHCompressorConfigType){
    HAL_REVPHCompressorConfigType_kDisabled = 0,
    HAL_REVPHCompressorConfigType_kDigital = 1,
    HAL_REVPHCompressorConfigType_kAnalog = 2,
    HAL_REVPHCompressorConfigType_kHybrid = 3,
};

/**
 * Storage for REV PH Version
 */
struct HAL_REVPHVersion {
  uint32_t firmwareMajor;
  uint32_t firmwareMinor;
  uint32_t firmwareFix;
  uint32_t hardwareMinor;
  uint32_t hardwareMajor;
  uint32_t uniqueId;
};

/**
 * Storage for compressor config
 */
struct HAL_REVPHCompressorConfig {
  double minAnalogVoltage;
  double maxAnalogVoltage;
  HAL_Bool forceDisable;
  HAL_Bool useDigital;
};

/**
 * Storage for REV PH Faults
 */
struct HAL_REVPHFaults {
  uint32_t channel0Fault : 1;
  uint32_t channel1Fault : 1;
  uint32_t channel2Fault : 1;
  uint32_t channel3Fault : 1;
  uint32_t channel4Fault : 1;
  uint32_t channel5Fault : 1;
  uint32_t channel6Fault : 1;
  uint32_t channel7Fault : 1;
  uint32_t channel8Fault : 1;
  uint32_t channel9Fault : 1;
  uint32_t channel10Fault : 1;
  uint32_t channel11Fault : 1;
  uint32_t channel12Fault : 1;
  uint32_t channel13Fault : 1;
  uint32_t channel14Fault : 1;
  uint32_t channel15Fault : 1;
  uint32_t compressorOverCurrent : 1;
  uint32_t compressorOpen : 1;
  uint32_t solenoidOverCurrent : 1;
  uint32_t brownout : 1;
  uint32_t canWarning : 1;
  uint32_t hardwareFault : 1;
};

/**
 * Storage for REV PH Sticky Faults
 */
struct HAL_REVPHStickyFaults {
  uint32_t compressorOverCurrent : 1;
  uint32_t compressorOpen : 1;
  uint32_t solenoidOverCurrent : 1;
  uint32_t brownout : 1;
  uint32_t canWarning : 1;
  uint32_t canBusOff : 1;
  uint32_t hasReset : 1;
};

#ifdef __cplusplus
extern "C" {
#endif

HAL_REVPHHandle HAL_InitializeREVPH(int32_t module,
                                    const char* allocationLocation,
                                    int32_t* status);

void HAL_FreeREVPH(HAL_REVPHHandle handle);

HAL_Bool HAL_CheckREVPHSolenoidChannel(int32_t channel);
HAL_Bool HAL_CheckREVPHModuleNumber(int32_t module);

HAL_Bool HAL_GetREVPHCompressor(HAL_REVPHHandle handle, int32_t* status);
void HAL_SetREVPHCompressorConfig(HAL_REVPHHandle handle,
                                  const HAL_REVPHCompressorConfig* config,
                                  int32_t* status);
void HAL_SetREVPHClosedLoopControlDisabled(HAL_REVPHHandle handle,
                                           int32_t* status);
void HAL_SetREVPHClosedLoopControlDigital(HAL_REVPHHandle handle,
                                          int32_t* status);
void HAL_SetREVPHClosedLoopControlAnalog(HAL_REVPHHandle handle,
                                         double minAnalogVoltage,
                                         double maxAnalogVoltage,
                                         int32_t* status);
void HAL_SetREVPHClosedLoopControlHybrid(HAL_REVPHHandle handle,
                                         double minAnalogVoltage,
                                         double maxAnalogVoltage,
                                         int32_t* status);
HAL_REVPHCompressorConfigType HAL_GetREVPHCompressorConfig(
    HAL_REVPHHandle handle, int32_t* status);
HAL_Bool HAL_GetREVPHPressureSwitch(HAL_REVPHHandle handle, int32_t* status);
double HAL_GetREVPHCompressorCurrent(HAL_REVPHHandle handle, int32_t* status);
double HAL_GetREVPHAnalogVoltage(HAL_REVPHHandle handle, int32_t channel,
                                 int32_t* status);
double HAL_GetREVPHVoltage(HAL_REVPHHandle handle, int32_t* status);
double HAL_GetREVPH5VVoltage(HAL_REVPHHandle handle, int32_t* status);
double HAL_GetREVPHSolenoidCurrent(HAL_REVPHHandle handle, int32_t* status);
double HAL_GetREVPHSolenoidVoltage(HAL_REVPHHandle handle, int32_t* status);
void HAL_GetREVPHVersion(HAL_REVPHHandle handle, HAL_REVPHVersion* version,
                         int32_t* status);

int32_t HAL_GetREVPHSolenoids(HAL_REVPHHandle handle, int32_t* status);
void HAL_SetREVPHSolenoids(HAL_REVPHHandle handle, int32_t mask, int32_t values,
                           int32_t* status);

void HAL_FireREVPHOneShot(HAL_REVPHHandle handle, int32_t index, int32_t durMs,
                          int32_t* status);

void HAL_GetREVPHFaults(HAL_REVPHHandle handle, HAL_REVPHFaults* faults,
                        int32_t* status);

void HAL_GetREVPHStickyFaults(HAL_REVPHHandle handle,
                              HAL_REVPHStickyFaults* stickyFaults,
                              int32_t* status);

void HAL_ClearREVPHStickyFaults(HAL_REVPHHandle handle, int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
