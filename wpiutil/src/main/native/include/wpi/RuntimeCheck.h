// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/string.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t WPI_IsRuntimeValid(uint32_t* FoundMajor, uint32_t* FoundMinor,
                           uint32_t* ExpectedMajor, uint32_t* ExpectedMinor,
                           WPI_String* RuntimePath);

#ifdef __cplusplus
}  // extern "C"
#endif
