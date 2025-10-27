// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/util/print.hpp>

#include "wpi/util/SmallString.hpp"

int main() {
  wpi::SmallString<128> v1("Hello");
  wpi::print("{}\n", v1.str());
}
