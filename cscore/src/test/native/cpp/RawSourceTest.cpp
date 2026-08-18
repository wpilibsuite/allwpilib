// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/cs/RawSource.hpp"

#include <catch2/catch_test_macros.hpp>

namespace wpi::cs {

TEST_CASE("RawSourceTest CreateEmpty", "[cscore][raw-source]") {
  RawSource source;
}

TEST_CASE("RawSourceTest Create", "[cscore][raw-source]") {
  RawSource source("test", wpi::util::PixelFormat::BGR, 640, 480, 30);
}

}  // namespace wpi::cs
