// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/Alert.h>

#include <chrono>
#include <string>

#include <fmt/format.h>
#include <gtest/gtest.h>

TEST(AlertsTest, Smoke) {
  { frc::Alert("Thing A", frc::Alert::AlertType::kError); }
  frc::Alert a{"", frc::Alert::AlertType::kInfo};
  {
    std::string s = fmt::format(
        "{}", std::chrono::steady_clock::now().time_since_epoch().count());
    a = frc::Alert(s, frc::Alert::AlertType::kError);
  }
}
