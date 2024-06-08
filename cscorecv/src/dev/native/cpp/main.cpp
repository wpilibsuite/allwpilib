// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <fmt/core.h>

#include "cscore.h"

int main() {
  fmt::print("{}\n", cs::GetHostname());
}
