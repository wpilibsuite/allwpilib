/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>

#include "llvm/SmallVector.h"
#include "llvm/StringRef.h"
#include "support/hostname.h"

int main() {
  llvm::StringRef v1("Hello");
  std::cout << v1.lower() << std::endl;
}
