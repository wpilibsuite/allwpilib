// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include <gtest/gtest.h>

#include "wpi/backend/NetworkTablesTelemetryBackend.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/StructTopic.hpp"
#include "wpi/telemetry/Telemetry.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"

class NetworkTablesTelemetryBackendTest : public ::testing::Test {
 public:
  NetworkTablesTelemetryBackendTest()
      : inst{wpi::nt::NetworkTableInstance::Create()},
        backend{std::make_shared<wpi::backend::NetworkTablesTelemetryBackend>(
            inst, "/Telemetry")} {
    wpi::TelemetryRegistry::Reset();
    wpi::TelemetryRegistry::RegisterBackend("", backend);
  }

  ~NetworkTablesTelemetryBackendTest() override {
    wpi::TelemetryRegistry::Reset();
    wpi::nt::NetworkTableInstance::Destroy(inst);
  }

  wpi::nt::NetworkTableInstance inst;
  std::shared_ptr<wpi::backend::NetworkTablesTelemetryBackend> backend;
};

TEST_F(NetworkTablesTelemetryBackendTest, PublishesStruct) {
  const wpi::math::Translation2d value{1.25_m, 2.5_m};
  auto sub = inst.GetStructTopic<wpi::math::Translation2d>(
                     "/Telemetry/translation")
                 .Subscribe({});

  wpi::Telemetry::Log("translation", value);

  auto logged = sub.Get();
  EXPECT_EQ(value.X(), logged.X());
  EXPECT_EQ(value.Y(), logged.Y());
}
