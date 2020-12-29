// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <iostream>

#include "wpi/SmallVector.h"
#include "wpi/StringRef.h"
#include "wpi/hostname.h"

int main() {
  wpi::StringRef v1("Hello");
  std::cout << v1.lower() << std::endl;
}
