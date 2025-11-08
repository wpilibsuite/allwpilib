// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/print.hpp"

#include "wpi/cs/cscore.h"

int main() {
  wpi::print("{}\n", cs::GetHostname());
}
