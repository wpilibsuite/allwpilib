// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunable/Selectable.hpp"

#include <format>
#include <memory>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/tunable/MockTunableBackend.hpp"
#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/Tunables.hpp"

class SelectableFixture {
 public:
  SelectableFixture() { wpi::TunableRegistry::RegisterBackend("", backend); }

  ~SelectableFixture() { wpi::TunableRegistry::Reset(); }

  std::shared_ptr<wpi::MockTunableBackend> backend =
      std::make_shared<wpi::MockTunableBackend>();
};

TEST_CASE_METHOD(SelectableFixture, "SelectableTest ReturnsSelected",
                 "[tunable][selectable]") {
  for (int param = 0; param <= 3; ++param) {
    wpi::Selectable<int> chooser;
    for (int i = 1; i <= 3; i++) {
      chooser.Add(std::to_string(i), i);
    }
    chooser.AddDefault("0", 0);

    wpi::Tunables::Publish(std::format("ReturnsSelectedChooser{}", param),
                           chooser);
    wpi::TunableRegistry::Update();
    backend->SetString(std::format("/ReturnsSelectedChooser{}/selected", param),
                       std::to_string(param));
    wpi::TunableRegistry::Update();
    CHECK(chooser.GetSelected() == param);
  }
}

TEST_CASE_METHOD(SelectableFixture,
                 "SelectableTest PublishesEntriesAndMetadata",
                 "[tunable][selectable]") {
  wpi::Selectable<int> chooser;
  chooser.Add("one", 1);
  chooser.AddDefault("two", 2);

  CHECK(chooser.GetTunableType() == "Selectable");

  CHECK(chooser.GetSelected() == 2);

  wpi::Tunables::Publish("MetadataChooser", chooser);
  backend->SetString("/MetadataChooser/selected", "one");
  wpi::TunableRegistry::Update();

  CHECK(chooser.GetSelected() == 1);
}

TEST_CASE("SelectableTest DefaultIsReturnedOnNoSelect",
          "[tunable][selectable]") {
  wpi::Selectable<int> chooser;

  for (int i = 1; i <= 3; i++) {
    chooser.Add(std::to_string(i), i);
  }

  // Use 4 here rather than 0 to make sure it's not default-init int.
  chooser.AddDefault("4", 4);

  CHECK(chooser.GetSelected() == 4);
}

TEST_CASE("SelectableTest DefaultConstructableIsReturnedOnNoSelectAndNoDefault",
          "[tunable][selectable]") {
  wpi::Selectable<int> chooser;

  for (int i = 1; i <= 3; i++) {
    chooser.Add(std::to_string(i), i);
  }

  CHECK(chooser.GetSelected() == 0);
}

TEST_CASE_METHOD(SelectableFixture, "SelectableTest ChangeListener",
                 "[tunable][selectable]") {
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

  CHECK(currentVal == 3);
}

TEST_CASE_METHOD(SelectableFixture,
                 "SelectableTest ListenerIsNotCalledForUnknownSelection",
                 "[tunable][selectable]") {
  wpi::Selectable<int> chooser;
  chooser.Add("one", 1);
  int currentVal = 0;
  chooser.OnChange([&](int val) { currentVal = val; });

  wpi::Tunables::Publish("UnknownSelectionChooser", chooser);
  backend->SetString("/UnknownSelectionChooser/selected", "missing");
  wpi::TunableRegistry::Update();

  CHECK(currentVal == 0);
  CHECK(chooser.GetSelected() == 0);
}

TEST_CASE_METHOD(SelectableFixture,
                 "SelectableTest ListenerReplacementUsesLatestListener",
                 "[tunable][selectable]") {
  wpi::Selectable<int> chooser;
  chooser.Add("one", 1);
  int first = 0;
  int second = 0;
  chooser.OnChange([&](int val) { first = val; });
  chooser.OnChange([&](int val) { second = val; });

  wpi::Tunables::Publish("ListenerReplacementChooser", chooser);
  backend->SetString("/ListenerReplacementChooser/selected", "one");
  wpi::TunableRegistry::Update();

  CHECK(first == 0);
  CHECK(second == 1);
}

TEST_CASE_METHOD(
    SelectableFixture,
    "SelectableTest DuplicateOptionReplacesValueWithoutDuplicatingOptions",
    "[tunable][selectable]") {
  wpi::Selectable<int> chooser;
  chooser.Add("mode", 1);
  chooser.Add("mode", 2);

  wpi::Tunables::Publish("DuplicateChooser", chooser);
  backend->SetString("/DuplicateChooser/selected", "mode");
  wpi::TunableRegistry::Update();

  CHECK(chooser.GetSelected() == 2);
}

TEST_CASE_METHOD(
    SelectableFixture,
    "SelectableTest ClearResetsOptionsAndDefaultButPreservesSelectionName",
    "[tunable][selectable]") {
  wpi::Selectable<int> chooser;
  chooser.AddDefault("one", 1);
  chooser.Add("two", 2);
  wpi::Tunables::Publish("ClearChooser", chooser);
  backend->SetString("/ClearChooser/selected", "two");
  wpi::TunableRegistry::Update();

  chooser.Clear();
  CHECK(chooser.GetSelected() == 0);

  chooser.Add("two", 22);
  CHECK(chooser.GetSelected() == 22);
}

TEST_CASE("SelectableTest SharedPtrSelectionReturnsWeakPtr",
          "[tunable][selectable]") {
  auto selected = std::make_shared<int>(5);
  wpi::Selectable<std::shared_ptr<int>> chooser;
  chooser.Add("selected", selected);

  auto weakSelected = chooser.GetSelected();
  CHECK(weakSelected.expired());

  chooser.AddDefault("default", std::make_shared<int>(6));
  CHECK(*chooser.GetSelected().lock() == 6);
}
