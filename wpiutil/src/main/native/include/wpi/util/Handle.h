// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#define WPI_INVALID_HANDLE 0

/**
 * Generic handle for all WPI handle-based interfaces.
 *
 * Handle data layout:
 * - Bits 0-23:  Type-specific
 * - Bits 24-30: Type
 * - Bit 31:     Error
 */
typedef int32_t WPI_Handle;  // NOLINT
