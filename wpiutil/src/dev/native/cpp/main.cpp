// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/SmallString.hpp"
#include "wpi/util/print.hpp"

int main() {
  wpi::util::SmallString<128> v1("Hello");
  wpi::util::print("{}\n", v1.str());
}
