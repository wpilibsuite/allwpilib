// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_session.hpp>

#include "aos/init.h"

int main(int argc, char** argv) {
  // AOS requires this before anything creates an event loop.
  aos::InitEmbedded();
  return Catch::Session().run(argc, argv);
}
