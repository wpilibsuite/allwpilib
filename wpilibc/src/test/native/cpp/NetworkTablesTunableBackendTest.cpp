// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include <gtest/gtest.h>

#include "wpi/backend/NetworkTablesTunableBackend.hpp"
#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/tunable/Tunable.hpp"
#include "wpi/tunable/TunableConfig.hpp"
#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/Tunables.hpp"

class NetworkTablesTunableBackendTest : public ::testing::Test {
 public:
  NetworkTablesTunableBackendTest()
      : inst{wpi::nt::NetworkTableInstance::Create()},
        backend{std::make_shared<wpi::backend::NetworkTablesTunableBackend>(
            inst, "/Tunables")} {
    wpi::TunableRegistry::Reset();
    wpi::TunableRegistry::RegisterBackend("", backend);
  }

  ~NetworkTablesTunableBackendTest() override {
    wpi::TunableRegistry::Reset();
    wpi::nt::NetworkTableInstance::Destroy(inst);
  }

  wpi::nt::NetworkTableInstance inst;
  std::shared_ptr<wpi::backend::NetworkTablesTunableBackend> backend;
};

TEST_F(NetworkTablesTunableBackendTest, PublishesAndTunesDouble) {
  wpi::TunableDouble value{1.0};
  wpi::Tunables::Publish("foo", value);

  auto sub = inst.GetDoubleTopic("/Tunables/foo").Subscribe(0.0);
  EXPECT_EQ(sub.Get(), 1.0);

  auto pub = inst.GetDoubleTopic("/Tunables/foo").Publish();
  pub.Set(2.0);
  wpi::TunableRegistry::Update();

  EXPECT_EQ(value.Get(), 2.0);
}

TEST_F(NetworkTablesTunableBackendTest, PublishesRobustDouble) {
  wpi::TunableConfig config;
  config.robust = true;
  wpi::TunableDouble value{1.0, config};
  wpi::Tunables::Publish("foo", value);

  auto sub = inst.GetDoubleTopic("/Tunables/foo/value").Subscribe(0.0);
  EXPECT_EQ(sub.Get(), 1.0);

  auto pub = inst.GetDoubleTopic("/Tunables/foo/tune").Publish();
  pub.Set(2.0);
  wpi::TunableRegistry::Update();

  EXPECT_EQ(value.Get(), 2.0);
  EXPECT_EQ(sub.Get(), 2.0);
}
