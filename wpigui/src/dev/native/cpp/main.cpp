// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpigui.h"

int main() {
  wpi::gui::CreateContext();
  wpi::gui::Initialize("Hello World", 1024, 768);
  wpi::gui::Main();
}
