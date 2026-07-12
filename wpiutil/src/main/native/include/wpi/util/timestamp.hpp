// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

namespace wpi::util {

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

}  // namespace wpi::util
