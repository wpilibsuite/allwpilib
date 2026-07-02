// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string_view>

#include <catch2/catch_session.hpp>

#include "wpi/hal/HAL.h"

namespace {

bool IsCatchListCommand(int argc, char** argv) {
  for (int i = 1; i < argc; ++i) {
    std::string_view arg{argv[i]};
    if (arg == "--list-tests" || arg == "--list-tags" ||
        arg == "--list-reporters" || arg == "--list-listeners") {
      return true;
    }
  }
  return false;
}

}  // namespace

int main(int argc, char** argv) {
  if (!IsCatchListCommand(argc, argv)) {
    HAL_Initialize(500, 0);
  }
  return Catch::Session().run(argc, argv);
}
