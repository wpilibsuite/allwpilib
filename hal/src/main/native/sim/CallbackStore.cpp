/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "simulation/CallbackStore.h"

void frc::sim::CallbackStoreThunk(const char* name, void* param,
                                  const HAL_Value* value) {
  reinterpret_cast<CallbackStore*>(param)->callback(name, value);
}
