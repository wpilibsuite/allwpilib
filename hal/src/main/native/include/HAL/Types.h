/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
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

typedef HAL_Handle HAL_CANHandle;

typedef int32_t HAL_Bool;

// credit to czipperz on GitHub for the idea
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
typedef enum {
    HAL_AccelerometerRange_k2G = 0,
    HAL_AccelerometerRange_k4G = 1,
    HAL_AccelerometerRange_k8G = 2
} HAL_AccelerometerRange;
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
 * In C, enums are basically macros for integral types. This means that the width
 * of the integer backing the enum doesn't matter, as long as the integral 
 * conversion works crossing a function barrier.
 * 
 * The C++ code has to name the enumeration in order to use it as a
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
#define HAL_ENUM_END(name, type) ; // NOLINT
#else
#define HAL_ENUM_START(name, type) typedef enum
#define HAL_ENUM_END(name, type) name;
#endif

/** @def HAL_ENUM_I32_START(name)
 *
 * @link HAL_ENUM_START @endlink but uses @c int32_t as the type.
 */
/** @def HAL_ENUM_I32_END(name)
 *
 * @link HAL_ENUM_END @endlink but uses @c int32_t as the type.
 */
#define HAL_ENUM_I32_START(name) HAL_ENUM_START(name, int32_t)
#define HAL_ENUM_I32_END(name) HAL_ENUM_END(name, int32_t)
