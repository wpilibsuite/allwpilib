/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>

namespace hal {
namespace init {

extern std::atomic_bool HAL_IsInitialized;

void RunInitialize();

static inline void CheckInit() {
  if (HAL_IsInitialized.load(std::memory_order_relaxed)) return;
  RunInitialize();
}

extern void InitializeCommonI2C();

}  // namespace init
}  // namespace hal
