// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/*
This example shows how to use the HAL directly, and what is needed to run a
basic program. The sample is compiled in C, however the functionality works from
C++ as well.

The HAL is considered a stable but changeable API. The API is stable during a
season, and is safe to use for events. However, between seasons there might be
changes to the API. This is an advanced sample, and should only be used by users
that want even more control over what code runs on their robot.
*/

#include <stdio.h>

#include <hal/HAL.h>
#include "hal/DriverStationTypes.h"

HAL_RobotMode getDSMode(void) {
  // Get Robot State
  HAL_ControlWord word;
  HAL_GetControlWord(&word);
  int64_t opmode = HAL_GetOpMode();

  // We send the observes, otherwise the DS disables
  HAL_ObserveUserProgramOpMode(opmode, word.enabled);
  return word.enabled ? word.robotMode : HAL_ROBOTMODE_UNKNOWN;
}

int main(void) {
  // Must initialize the HAL, 500ms timeout
  HAL_Bool initialized = HAL_Initialize(500, 0);
  if (!initialized) {
    printf("Failed to initialize the HAL\n");
    return 1;
  }

  int32_t status = 0;

  // Create an opmode per robot mode
  struct WPI_String name;
  WPI_InitString(&name, "Auto");
  HAL_AddOpMode(HAL_ROBOTMODE_AUTONOMOUS, &name, NULL, NULL, -1, -1);
  WPI_InitString(&name, "Teleop");
  HAL_AddOpMode(HAL_ROBOTMODE_TELEOPERATED, &name, NULL, NULL, -1, -1);
  WPI_InitString(&name, "Test");
  HAL_AddOpMode(HAL_ROBOTMODE_TEST, &name, NULL, NULL, -1, -1);

  // For DS to see valid robot code
  HAL_ObserveUserProgramStarting();

  // Create a Motor Controller
  status = 0;
  HAL_DigitalHandle pwmPort = HAL_InitializePWMPort(1, NULL, &status);

  if (status != 0) {
    const char* message = HAL_GetLastError(&status);
    printf("%s\n", message);
    return 1;
  }

  // Create an Input
  status = 0;
  HAL_DigitalHandle dio = HAL_InitializeDIOPort(2, 1, NULL, &status);

  if (status != 0) {
    const char* message = HAL_GetLastError(&status);
    printf("%s\n", message);
    HAL_FreePWMPort(pwmPort);
    return 1;
  }

  WPI_EventHandle eventHandle = WPI_CreateEvent(0, 0);
  HAL_ProvideNewDataEventHandle(eventHandle);

  while (1) {
    // Wait for DS data, with a timeout
    int timed_out = 0;
    int signaled = WPI_WaitForObjectTimeout(eventHandle, 1.0, &timed_out);
    if (!signaled) {
      // Do something here on no packet
      continue;
    }

    HAL_RefreshDSData();

    HAL_RobotMode dsMode = getDSMode();
    switch (dsMode) {
      case HAL_ROBOTMODE_UNKNOWN:
        break;
      case HAL_ROBOTMODE_TELEOPERATED:
        status = 0;
        if (HAL_GetDIO(dio, &status)) {
          HAL_SetPWMPulseTimeMicroseconds(pwmPort, 2000, &status);
        } else {
          HAL_SetPWMPulseTimeMicroseconds(pwmPort, 1500, &status);
        }
        break;
      case HAL_ROBOTMODE_AUTONOMOUS:
        break;
      case HAL_ROBOTMODE_TEST:
        break;
      default:
        break;
    }
  }

  // Clean up resources
  HAL_RemoveNewDataEventHandle(eventHandle);
  WPI_DestroyEvent(eventHandle);

  status = 0;
  HAL_FreeDIOPort(dio);
  HAL_FreePWMPort(pwmPort);
}
