// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include "CommandTestBase.hpp"
#include "wpi/telemetry/MockTelemetryBackend.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"

using namespace wpi::cmd;

class CommandTelemetryTest : public CommandTestBase {
 protected:
  CommandTelemetryTest() {
    wpi::TelemetryRegistry::Reset();
    m_backend = std::make_shared<wpi::MockTelemetryBackend>();
    wpi::TelemetryRegistry::RegisterBackend("", m_backend);
  }

  ~CommandTelemetryTest() override { wpi::TelemetryRegistry::Reset(); }

  std::shared_ptr<wpi::MockTelemetryBackend> m_backend;
};

TEST_CASE_METHOD(CommandTelemetryTest,
                 "CommandTelemetryTest CommandLogsMetadataWithoutLeadingDots",
                 "[commandsv2][command]") {
  MockCommand command;
  command.SetName("renamed");

  command.LogTo(wpi::TelemetryRegistry::GetTable("command"));

  auto name =
      m_backend->GetLastValue<wpi::MockTelemetryBackend::LogStringValue>(
          "/command/name");
  REQUIRE(name);
  CHECK(name->value == "renamed");
  CHECK(m_backend->GetLastAction("/command/.name") == nullptr);

  auto isParented = m_backend->GetLastValue<bool>("/command/isParented");
  REQUIRE(isParented);
  CHECK_FALSE(*isParented);
  CHECK(m_backend->GetLastAction("/command/.isParented") == nullptr);
}
