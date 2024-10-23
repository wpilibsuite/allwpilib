// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/Alert.h>

#include <chrono>
#include <string>
#include <utility>

#include <fmt/format.h>
#include <gtest/gtest.h>

TEST(AlertsTest, Smoke) {
  // Make and destroy
  frc::Alert("One", frc::Alert::AlertType::kError);
  frc::Alert a{"Two", frc::Alert::AlertType::kInfo};
  a.Set(true);
  a.Set(false);
  // Move assign
  a = frc::Alert("Three", frc::Alert::AlertType::kWarning);
  // Move construct
  frc::Alert b{std::move(a)};
  {
    // Move assign with dynamic string
    std::string text = fmt::format(
        "{}", std::chrono::steady_clock::now().time_since_epoch().count());
    b = frc::Alert(text, frc::Alert::AlertType::kError);
  }
}
