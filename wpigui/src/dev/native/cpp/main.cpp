/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpigui.h"

int main() {
  wpi::gui::CreateContext();
  wpi::gui::Initialize("Hello World", 1024, 768);
  wpi::gui::Main();
}
