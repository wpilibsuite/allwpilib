// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_TIMESTAMP_H_
#define WPIUTIL_WPI_TIMESTAMP_H_

#include <stdint.h>

#ifdef __cplusplus
#include <memory>  // NOLINT

#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * De-initialize the on-Rio Now() implementation. No effect on non-Rio
 * platforms.
 */
void WPI_Impl_ShutdownNowRio(void);

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
 * Initialize the on-Rio Now() implementation to use the desktop timestamp.
 * No effect on non-Rio platforms. This should only be used for testing
 * purposes if the HAL is not available.
 */
void SetupNowDefaultOnRio();

/**
 * Initialize the on-Rio Now() implementation to use the FPGA timestamp.
 * No effect on non-Rio platforms. This is called by HAL_Initialize() and
 * thus should generally not be called by user code.
 */
#ifdef __FRC_ROBORIO__
template <typename T>
bool SetupNowRio(void* chipObjectLibrary, std::unique_ptr<T> hmbObject);
#else
template <typename T>
inline bool SetupNowRio(void*, std::unique_ptr<T>) {
  return true;
}
#endif

/**
 * Initialize the on-Rio Now() implementation to use the FPGA timestamp.
 * No effect on non-Rio platforms. This take an FPGA session that has
 * already been initialized, and is used from LabVIEW.
 */
bool SetupNowRio(uint32_t session);

/**
 * De-initialize the on-Rio Now() implementation. No effect on non-Rio
 * platforms.
 */
void ShutdownNowRio();
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
