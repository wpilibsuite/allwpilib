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

int getDSMode(void) {
  // Get Robot State
  HAL_ControlWord word;
  HAL_GetControlWord(&word);
  int mode = HAL_GetOpMode();

  // We send the observes, otherwise the DS disables
  HAL_ObserveUserProgramOpMode(mode);
  return mode;
}

int main(void) {
  // Must initialize the HAL, 500ms timeout
  HAL_Bool initialized = HAL_Initialize(500, 0);
  if (!initialized) {
    printf("Failed to initialize the HAL\n");
    return 1;
  }

  int32_t status = 0;

  // Create a couple of opmodes
  struct WPI_String name, category, description;
  WPI_InitString(&name, "teleop");
  WPI_InitString(&category, "");
  WPI_InitString(&description, "");
  int teleopMode = HAL_AddOpModeOption(&name, &category, &description, 0);
  WPI_InitString(&name, "auto");
  WPI_InitString(&category, "");
  WPI_InitString(&description, "");
  int autoMode = HAL_AddOpModeOption(&name, &category, &description, 0);

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

  // Set PWM config to standard servo speeds
  HAL_SetPWMConfigMicroseconds(pwmPort, 2000, 1501, 1500, 1499, 1000, &status);

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

    int dsMode = getDSMode();
    if (dsMode == 0) {
      // disabled
    } else if (dsMode == teleopMode) {
      status = 0;
      if (HAL_GetDIO(dio, &status)) {
        HAL_SetPWMSpeed(pwmPort, 1.0, &status);
      } else {
        HAL_SetPWMSpeed(pwmPort, 0, &status);
      }
    } else if (dsMode == autoMode) {
      // auto
    }
  }

  // Clean up resources
  HAL_RemoveNewDataEventHandle(eventHandle);
  WPI_DestroyEvent(eventHandle);

  status = 0;
  HAL_FreeDIOPort(dio);
  HAL_FreePWMPort(pwmPort);
}
