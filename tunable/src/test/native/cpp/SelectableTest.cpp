// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunable/Selectable.hpp"

#include <memory>
#include <string>

#include <fmt/format.h>
#include <gtest/gtest.h>

#include "wpi/tunable/MockTunableBackend.hpp"
#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/Tunables.hpp"

class SelectableTest : public ::testing::TestWithParam<int> {
 protected:
  void SetUp() override { wpi::TunableRegistry::RegisterBackend("", backend); }

  void TearDown() override { wpi::TunableRegistry::Reset(); }

  std::shared_ptr<wpi::MockTunableBackend> backend =
      std::make_shared<wpi::MockTunableBackend>();
};

TEST_P(SelectableTest, ReturnsSelected) {
  wpi::Selectable<int> chooser;
  for (int i = 1; i <= 3; i++) {
    chooser.Add(std::to_string(i), i);
  }
  chooser.AddDefault("0", 0);

  wpi::Tunables::Publish(fmt::format("ReturnsSelectedChooser{}", GetParam()),
                         chooser);
  wpi::TunableRegistry::Update();
  backend->SetString(
      fmt::format("/ReturnsSelectedChooser{}/selected", GetParam()),
      std::to_string(GetParam()));
  wpi::TunableRegistry::Update();
  EXPECT_EQ(GetParam(), chooser.GetSelected());
}

TEST(SelectableTest, DefaultIsReturnedOnNoSelect) {
  wpi::Selectable<int> chooser;

  for (int i = 1; i <= 3; i++) {
    chooser.Add(std::to_string(i), i);
  }

  // Use 4 here rather than 0 to make sure it's not default-init int.
  chooser.AddDefault("4", 4);

  EXPECT_EQ(4, chooser.GetSelected());
}

TEST(SelectableTest, DefaultConstructableIsReturnedOnNoSelectAndNoDefault) {
  wpi::Selectable<int> chooser;

  for (int i = 1; i <= 3; i++) {
    chooser.Add(std::to_string(i), i);
  }

  EXPECT_EQ(0, chooser.GetSelected());
}

TEST_F(SelectableTest, ChangeListener) {
  wpi::Selectable<int> chooser;
  for (int i = 1; i <= 3; i++) {
    chooser.Add(std::to_string(i), i);
  }
  int currentVal = 0;
  chooser.OnChange([&](int val) { currentVal = val; });

  wpi::Tunables::Publish("ChangeListenerChooser", chooser);
  wpi::TunableRegistry::Update();
  backend->SetString("/ChangeListenerChooser/selected", "3");
  wpi::TunableRegistry::Update();

  EXPECT_EQ(3, currentVal);
}

INSTANTIATE_TEST_SUITE_P(SelectableTests, SelectableTest,
                         ::testing::Values(0, 1, 2, 3));
