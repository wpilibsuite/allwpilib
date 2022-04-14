// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <fmt/core.h>

#include "ntcore.h"

int main() {
  auto myValue = nt::GetEntry(nt::GetDefaultInstance(), "MyValue");

  nt::SetEntryValue(myValue, nt::Value::MakeString("Hello World"));

  fmt::print("{}\n", nt::GetEntryValue(myValue)->GetString());
}
