// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/telemetry/TelemetryTable.hpp"  // NOLINT(build/include_order)

#include <memory>
#include <variant>

#include <gtest/gtest.h>

#include "wpi/telemetry/MockTelemetryBackend.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"

namespace telemetrytest {
struct TestStructADL {
  double x = 0;
  double y = 0;
};

void LogTo(wpi::TelemetryTable& table, std::string_view name,
           const TestStructADL& value) {
  auto& subtable = table.GetTable(name);
  subtable.Log("x", value.x);
  subtable.Log("y", value.y);
}

struct TestStructLoggable {
  double x = 0;
  double y = 0;

  void UpdateTelemetry(wpi::TelemetryTable& table) const {
    table.Log("x", x);
    table.Log("y", y);
  }
};

struct TestStructLoggableType : public TestStructLoggable {
  std::string_view GetTelemetryType() const { return "TestStructLoggableType"; }
};
}  // namespace telemetrytest

struct TelemetryTableTest : public ::testing::Test {
  void SetUp() override {
    wpi::TelemetryRegistry::Reset();
    wpi::TelemetryRegistry::RegisterBackend("", mock);
  }

  void TearDown() override { wpi::TelemetryRegistry::Reset(); }

  std::shared_ptr<wpi::MockTelemetryBackend> mock =
      std::make_shared<wpi::MockTelemetryBackend>();
};

TEST_F(TelemetryTableTest, LogADL) {
  wpi::TelemetryTable& table = wpi::TelemetryRegistry::GetTable("/");
  telemetrytest::TestStructADL val{1, 2};
  table.Log("testadl", val);
  auto actions = mock->GetActions();
  ASSERT_EQ(actions.size(), 2u);

  ASSERT_EQ(actions[0].path, "/testadl/x");
  ASSERT_TRUE(std::holds_alternative<double>(actions[0].value));
  ASSERT_EQ(std::get<double>(actions[0].value), 1);

  ASSERT_EQ(actions[1].path, "/testadl/y");
  ASSERT_TRUE(std::holds_alternative<double>(actions[1].value));
  ASSERT_EQ(std::get<double>(actions[1].value), 2);
}

TEST_F(TelemetryTableTest, LogLoggable) {
  wpi::TelemetryTable& table = wpi::TelemetryRegistry::GetTable("/");
  telemetrytest::TestStructLoggable val{1, 2};
  table.Log("test", val);
  auto actions = mock->GetActions();
  ASSERT_EQ(actions.size(), 2u);

  ASSERT_EQ(actions[0].path, "/test/x");
  ASSERT_TRUE(std::holds_alternative<double>(actions[0].value));
  ASSERT_EQ(std::get<double>(actions[0].value), 1);

  ASSERT_EQ(actions[1].path, "/test/y");
  ASSERT_TRUE(std::holds_alternative<double>(actions[1].value));
  ASSERT_EQ(std::get<double>(actions[1].value), 2);
}

TEST_F(TelemetryTableTest, LogLoggableType) {
  wpi::TelemetryTable& table = wpi::TelemetryRegistry::GetTable("/");
  telemetrytest::TestStructLoggableType val{1, 2};
  table.Log("test", val);
  ASSERT_EQ(table.GetTable("test").GetType(), "TestStructLoggableType");
  table.Log("test", val);
  auto actions = mock->GetActions();
  ASSERT_EQ(actions.size(), 5u);

  auto value = mock->GetLastValue<wpi::MockTelemetryBackend::LogStringValue>(
      "/test/.type");
  ASSERT_TRUE(value);
  ASSERT_EQ(value->value, "TestStructLoggableType");
}
