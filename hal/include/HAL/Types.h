/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#define HAL_kInvalidHandle 0

typedef int32_t HAL_Handle;

typedef HAL_Handle HAL_PortHandle;

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

typedef int32_t HAL_Bool;

/** @def HAL_ENUM_START(name, type)
 *
 * This macro is used before the start of a typed enum declaration:

@code{.cpp}
HAL_ENUM_START(HAL_AccelerometerRange, int32_t) {
    HAL_AccelerometerRange_k2G = 0,
    HAL_AccelerometerRange_k4G = 1,
    HAL_AccelerometerRange_k8G = 2
}
HAL_ENUM_END(HAL_AccelerometerRange, int32_t)
@endcode

 * In C this evaluates to:

@code{.c}
enum {
    HAL_AccelerometerRange_k2G = 0,
    HAL_AccelerometerRange_k4G = 1,
    HAL_AccelerometerRange_k8G = 2
};
typedef int32_t HAL_AccelerometerRange;
@endcode

 * While in C++ this evaluates to:

@code{.cpp}
enum HAL_AccelerometerRange : int32_t {
    HAL_AccelerometerRange_k2G = 0,
    HAL_AccelerometerRange_k4G = 1,
    HAL_AccelerometerRange_k8G = 2
};
@endcode

 * This defines the constants @c HAL_AccelerometerRange_k2G,
 * @c HAL_AccelerometerRange_k4G, and @c HAL_AccelerometerRange_k8G to @c 0,
 * @c 1, and @c 2, respectively.  It also makes a 32 bit signed integer type
 * called @c HAL_AccelerometerRange.
 *
 * We need the C version to use the typedefinition because otherwise we couldn't
 * get the enumeration to be exactly 32 bits, required by the implementation of
 * HAL.  The C++ code has to name the enumeration in order to use it as a
 * namespace, for example @c HAL_AccelerometerRange::HAL_AccelerometerRange_k2G
 * is used in code and therefore it must remain backwards compatible.
 */
/** @def HAL_ENUM_END(name, type)
 *
 * This macro is used after the body of a typed enum declaration.
 *
 * For more documentation see @link HAL_ENUM_START @endlink.
 */

#ifdef __cplusplus
#define HAL_ENUM_START(name, type) enum name : type
#define HAL_ENUM_END(name, type) ; /* NOLINT */
#else
#define HAL_ENUM_START(name, type) enum
#define HAL_ENUM_END(name, type) \
  ;                              \
  typedef type name;
#endif
