/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_TIMESTAMP_H_
#define WPIUTIL_WPI_TIMESTAMP_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}  // extern "C"
#endif

#ifdef __cplusplus
namespace wpi {

/**
 * The default implementation used for Now().
 * In general this is the time returned by the operating system.
 * @return Time in microseconds.
 */
uint64_t NowDefault(void);

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
uint64_t Now(void);

}  // namespace wpi
#endif

#endif  // WPIUTIL_WPI_TIMESTAMP_H_
