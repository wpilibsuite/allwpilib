// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("xrpVendordep native test binary loads", "[xrp][vendordep][smoke]") {}

int main(int argc, char** argv) {
  return Catch::Session().run(argc, argv);
}
