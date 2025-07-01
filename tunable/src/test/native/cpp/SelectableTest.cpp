// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunable/Selectable.hpp"

#include <format>
#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/tunable/MockTunableBackend.hpp"
#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/Tunables.hpp"

class SelectableParameterizedTest : public ::testing::TestWithParam<int> {
 protected:
  void SetUp() override { wpi::TunableRegistry::RegisterBackend("", backend); }

  void TearDown() override { wpi::TunableRegistry::Reset(); }

  std::shared_ptr<wpi::MockTunableBackend> backend =
      std::make_shared<wpi::MockTunableBackend>();
};

class SelectableFixture : public ::testing::Test {
 protected:
  void SetUp() override { wpi::TunableRegistry::RegisterBackend("", backend); }

  void TearDown() override { wpi::TunableRegistry::Reset(); }

  std::shared_ptr<wpi::MockTunableBackend> backend =
      std::make_shared<wpi::MockTunableBackend>();
};

TEST_P(SelectableParameterizedTest, ReturnsSelected) {
  wpi::Selectable<int> chooser;
  for (int i = 1; i <= 3; i++) {
    chooser.Add(std::to_string(i), i);
  }
  chooser.AddDefault("0", 0);

  wpi::Tunables::Publish(std::format("ReturnsSelectedChooser{}", GetParam()),
                         chooser);
  wpi::TunableRegistry::Update();
  backend->SetString(
      std::format("/ReturnsSelectedChooser{}/selected", GetParam()),
      std::to_string(GetParam()));
  wpi::TunableRegistry::Update();
  EXPECT_EQ(GetParam(), chooser.GetSelected());
}

TEST_F(SelectableFixture, PublishesEntriesAndMetadata) {
  wpi::Selectable<int> chooser;
  chooser.Add("one", 1);
  chooser.AddDefault("two", 2);

  EXPECT_EQ(chooser.GetTunableType(), "Selectable");

  EXPECT_EQ(2, chooser.GetSelected());

  wpi::Tunables::Publish("MetadataChooser", chooser);
  backend->SetString("/MetadataChooser/selected", "one");
  wpi::TunableRegistry::Update();

  EXPECT_EQ(1, chooser.GetSelected());
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

TEST_F(SelectableFixture, ChangeListener) {
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

TEST_F(SelectableFixture, ListenerIsNotCalledForUnknownSelection) {
  wpi::Selectable<int> chooser;
  chooser.Add("one", 1);
  int currentVal = 0;
  chooser.OnChange([&](int val) { currentVal = val; });

  wpi::Tunables::Publish("UnknownSelectionChooser", chooser);
  backend->SetString("/UnknownSelectionChooser/selected", "missing");
  wpi::TunableRegistry::Update();

  EXPECT_EQ(0, currentVal);
  EXPECT_EQ(0, chooser.GetSelected());
}

TEST_F(SelectableFixture, ListenerReplacementUsesLatestListener) {
  wpi::Selectable<int> chooser;
  chooser.Add("one", 1);
  int first = 0;
  int second = 0;
  chooser.OnChange([&](int val) { first = val; });
  chooser.OnChange([&](int val) { second = val; });

  wpi::Tunables::Publish("ListenerReplacementChooser", chooser);
  backend->SetString("/ListenerReplacementChooser/selected", "one");
  wpi::TunableRegistry::Update();

  EXPECT_EQ(0, first);
  EXPECT_EQ(1, second);
}

TEST_F(SelectableFixture, DuplicateOptionReplacesValueWithoutDuplicatingOptions) {
  wpi::Selectable<int> chooser;
  chooser.Add("mode", 1);
  chooser.Add("mode", 2);

  wpi::Tunables::Publish("DuplicateChooser", chooser);
  backend->SetString("/DuplicateChooser/selected", "mode");
  wpi::TunableRegistry::Update();

  EXPECT_EQ(2, chooser.GetSelected());
}

TEST_F(SelectableFixture, ClearResetsOptionsAndDefaultButPreservesSelectionName) {
  wpi::Selectable<int> chooser;
  chooser.AddDefault("one", 1);
  chooser.Add("two", 2);
  wpi::Tunables::Publish("ClearChooser", chooser);
  backend->SetString("/ClearChooser/selected", "two");
  wpi::TunableRegistry::Update();

  chooser.Clear();
  EXPECT_EQ(0, chooser.GetSelected());

  chooser.Add("two", 22);
  EXPECT_EQ(22, chooser.GetSelected());
}

TEST(SelectableTest, SharedPtrSelectionReturnsWeakPtr) {
  auto selected = std::make_shared<int>(5);
  wpi::Selectable<std::shared_ptr<int>> chooser;
  chooser.Add("selected", selected);

  auto weakSelected = chooser.GetSelected();
  EXPECT_TRUE(weakSelected.expired());

  chooser.AddDefault("default", std::make_shared<int>(6));
  EXPECT_EQ(*chooser.GetSelected().lock(), 6);
}

INSTANTIATE_TEST_SUITE_P(SelectableTests, SelectableParameterizedTest,
                         ::testing::Values(0, 1, 2, 3));
