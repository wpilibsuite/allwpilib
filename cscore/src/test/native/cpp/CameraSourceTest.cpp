// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/cs/HttpCamera.hpp"
#include "wpi/cs/cscore_cpp.hpp"

namespace wpi::cs {

TEST_CASE("CameraSourceTest HTTPCamera", "[cscore][camera-source]") {
  auto source = HttpCamera("camera", "http://localhost:8000");
  wpi::cs::Shutdown();
}

}  // namespace wpi::cs
