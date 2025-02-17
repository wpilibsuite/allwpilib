// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/telemetry/TelemetryTable.h"  // NOLINT(build/include_order)

#include <gtest/gtest.h>

namespace telemetrytest {
struct TestStructADL {
  double x = 0;
  double y = 0;
};

void Log(wpi::TelemetryTable& table, const TestStructADL& value) {
  table.Log("x", value.x);
  table.Log("y", value.y);
}
}  // namespace telemetrytest

TEST(TelemetryTableTest, LogADL) {
  wpi::TelemetryTable& table = wpi::TelemetryRegistry::GetTable("/");
  telemetrytest::TestStructADL val;
  table.Log("testadl", val);
}

