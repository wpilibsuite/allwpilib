/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>

#include <windows.h>

int main() {
  HMODULE module = LoadLibraryA("wpiutiljni.dll");

  if (!module) {
    DWORD lastError = GetLastError();
    std::cout << (int)lastError << std::endl;

  } else {
    std::cout << "Loaded correctly" << std::endl;
  }
}
