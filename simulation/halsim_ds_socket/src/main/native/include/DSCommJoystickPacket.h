/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once
#include <vector>

typedef struct {
  std::vector<int16_t> axes;
  uint8_t button_count;
  uint32_t buttons;
  std::vector<int16_t> povs;
} DSCommJoystickPacket;
