// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/cs/RawSource.hpp"

#include <gtest/gtest.h>

namespace wpi::cs {

TEST(RawSourceTest, CreateEmpty) {
  RawSource source;
}

TEST(RawSourceTest, Create) {
  RawSource source("test", wpi::util::PixelFormat::BGR, 640, 480, 30);
}

}  // namespace wpi::cs
