// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/cs/HttpCamera.hpp"
#include "wpi/cs/cscore_cpp.hpp"

namespace wpi::cs {

class CameraSourceTest : public ::testing::Test {
 protected:
  CameraSourceTest() = default;
};

TEST_F(CameraSourceTest, HTTPCamera) {
  auto source = HttpCamera("axis", "http://localhost:8000");
  wpi::cs::Shutdown();
}

}  // namespace wpi::cs
