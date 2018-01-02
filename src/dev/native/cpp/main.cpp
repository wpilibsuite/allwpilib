/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017-2018. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>

#include "ntcore.h"

int main() {
  auto myValue = nt::GetEntry(nt::GetDefaultInstance(), "MyValue");

  nt::SetEntryValue(myValue, nt::Value::MakeString("Hello World"));

  std::cout << nt::GetEntryValue(myValue)->GetString() << std::endl;
}
