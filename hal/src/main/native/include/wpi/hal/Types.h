// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

/**
 * @defgroup hal_types Type Definitions
 * @ingroup hal_capi
 * @{
 */

#define HAL_kInvalidHandle 0

typedef int32_t HAL_Handle;

typedef HAL_Handle HAL_AnalogInputHandle;

typedef HAL_Handle HAL_AnalogOutputHandle;

typedef HAL_Handle HAL_AnalogTriggerHandle;

typedef HAL_Handle HAL_CompressorHandle;

typedef HAL_Handle HAL_CounterHandle;

typedef HAL_Handle HAL_DigitalHandle;

typedef HAL_Handle HAL_DigitalPWMHandle;

typedef HAL_Handle HAL_EncoderHandle;

typedef HAL_Handle HAL_FPGAEncoderHandle;

typedef HAL_Handle HAL_GyroHandle;

typedef HAL_Handle HAL_InterruptHandle;

typedef HAL_Handle HAL_NotifierHandle;

typedef HAL_Handle HAL_RelayHandle;

typedef HAL_Handle HAL_SolenoidHandle;

typedef HAL_Handle HAL_SerialPortHandle;

typedef HAL_Handle HAL_CANHandle;

typedef HAL_Handle HAL_SimDeviceHandle;

typedef HAL_Handle HAL_SimValueHandle;

typedef HAL_Handle HAL_DMAHandle;

typedef HAL_Handle HAL_DutyCycleHandle;

typedef HAL_Handle HAL_AddressableLEDHandle;

typedef HAL_CANHandle HAL_PDPHandle;

typedef HAL_Handle HAL_PowerDistributionHandle;

typedef HAL_Handle HAL_CTREPCMHandle;

typedef HAL_Handle HAL_REVPDHHandle;

typedef HAL_Handle HAL_REVPHHandle;

typedef HAL_Handle HAL_CANStreamHandle;

typedef int32_t HAL_Bool;

#ifdef __cplusplus
#define HAL_ENUM_WITH_UNDERLYING_TYPE(name, type) enum name : type
#elif defined(__clang__)
#define HAL_ENUM_WITH_UNDERLYING_TYPE(name, type) \
  enum name : type;                               \
  typedef enum name name;                         \
  enum name : type
#else
#define HAL_ENUM_WITH_UNDERLYING_TYPE(name, type) \
  typedef type name;                              \
  enum name
#endif

#define HAL_ENUM(name) HAL_ENUM_WITH_UNDERLYING_TYPE(name, int32_t)

/** @} */
