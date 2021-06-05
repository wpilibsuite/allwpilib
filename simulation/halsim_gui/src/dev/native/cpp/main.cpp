// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <thread>

#include <hal/CTREPCM.h>
#include <hal/HALBase.h>
#include <hal/Main.h>

int main() {
  HAL_Initialize(500, 0);

  int32_t status = 0;
  HAL_CTREPCMHandle handle = HAL_InitializeCTREPCM(0, nullptr, &status);

  HAL_SetCTREPCMSolenoids(handle, 0xFF, 0x1, &status);

  if (HAL_HasMain()) {
    HAL_RunMain();
  }

  HAL_FreeCTREPCM(handle);
}
