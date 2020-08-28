/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "GuiUtil.h"

void halsimgui::MaxFit(ImVec2* min, ImVec2* max, float width, float height) {
  float destWidth = max->x - min->x;
  float destHeight = max->y - min->y;
  if (width == 0 || height == 0) return;
  if (destWidth * height > destHeight * width) {
    float outputWidth = width * destHeight / height;
    min->x += (destWidth - outputWidth) / 2;
    max->x -= (destWidth - outputWidth) / 2;
  } else {
    float outputHeight = height * destWidth / width;
    min->y += (destHeight - outputHeight) / 2;
    max->y -= (destHeight - outputHeight) / 2;
  }
}
