// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_TIMESTAMP_H_
#define WPIUTIL_WPI_TIMESTAMP_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the on-Rio Now() implementation to use the FPGA timestamp.
 * No effect on non-Rio platforms. This is called by HAL_Initialize() and
 * thus should generally not be called by user code.
 */
void WPI_Impl_SetupNowRio(void);

/**
 * The default implementation used for Now().
 * In general this is the time returned by the operating system.
 * @return Time in microseconds.
 */
uint64_t WPI_NowDefault(void);

/**
 * Set the implementation used by WPI_Now().
 * The implementation must return monotonic time in microseconds to maintain
 * the contract of WPI_Now().
 * @param func Function called by WPI_Now() to return the time.
 */
void WPI_SetNowImpl(uint64_t (*func)(void));

/**
 * Return a value representing the current time in microseconds.
 * The epoch is not defined.
 * @return Time in microseconds.
 */
uint64_t WPI_Now(void);

/**
 * Return the current system time in microseconds since the Unix epoch
 * (January 1st, 1970 00:00 UTC).
 *
 * @return Time in microseconds.
 */
uint64_t WPI_GetSystemTime(void);

#ifdef __cplusplus
}  // extern "C"
#endif

#ifdef __cplusplus
namespace wpi {

namespace impl {
/**
 * Initialize the on-Rio Now() implementation to use the FPGA timestamp.
 * No effect on non-Rio platforms. This is called by HAL_Initialize() and
 * thus should generally not be called by user code.
 */
void SetupNowRio();
}  // namespace impl

/**
 * The default implementation used for Now().
 * In general this is the time returned by the operating system.
 * @return Time in microseconds.
 */
uint64_t NowDefault();

/**
 * Set the implementation used by Now().
 * The implementation must return monotonic time in microseconds to maintain
 * the contract of Now().
 * @param func Function called by Now() to return the time.
 */
void SetNowImpl(uint64_t (*func)());

/**
 * Return a value representing the current time in microseconds.
 * This is a monotonic clock with an undefined epoch.
 * @return Time in microseconds.
 */
uint64_t Now();

/**
 * Return the current system time in microseconds since the Unix epoch
 * (January 1st, 1970 00:00 UTC).
 *
 * @return Time in microseconds.
 */
uint64_t GetSystemTime();

}  // namespace wpi
#endif

#endif  // WPIUTIL_WPI_TIMESTAMP_H_
