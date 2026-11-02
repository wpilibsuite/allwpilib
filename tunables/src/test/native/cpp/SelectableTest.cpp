// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunables/Selectable.hpp"

#include <format>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/tunables/MockTunableBackend.hpp"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/Tunables.hpp"

class SelectableFixture {
 public:
  SelectableFixture() {
    wpi::tunables::TunableRegistry::RegisterBackend("", backend);
  }

  ~SelectableFixture() { wpi::tunables::TunableRegistry::Reset(); }

  std::shared_ptr<wpi::tunables::MockTunableBackend> backend =
      std::make_shared<wpi::tunables::MockTunableBackend>();
};

TEST_CASE_METHOD(SelectableFixture, "SelectableTest ReturnsSelected",
                 "[tunable][selectable]") {
  for (int param = 0; param <= 3; ++param) {
    wpi::tunables::Selectable<int> chooser;
    for (int i = 1; i <= 3; i++) {
      chooser.Add(std::to_string(i), i);
    }
    chooser.AddDefault("0", 0);

    wpi::tunables::Publish(std::format("ReturnsSelectedChooser{}", param),
                           chooser);
    wpi::tunables::TunableRegistry::Update();
    backend->SetString(std::format("/ReturnsSelectedChooser{}/selected", param),
                       std::to_string(param));
    wpi::tunables::TunableRegistry::Update();
    CHECK(chooser.GetSelected() == param);
  }
}

TEST_CASE_METHOD(SelectableFixture,
                 "SelectableTest PublishesEntriesAndMetadata",
                 "[tunable][selectable]") {
  wpi::tunables::Selectable<int> chooser;
  chooser.Add("one", 1);
  chooser.AddDefault("two", 2);

  CHECK(chooser.GetTunableType() == "Selectable");

  CHECK(chooser.GetSelected() == 2);

  wpi::tunables::Publish("MetadataChooser", chooser);
  CHECK(backend->GetString("/MetadataChooser/default") == "two");
  CHECK(backend->GetStringVector("/MetadataChooser/options") ==
        (std::vector<std::string>{"one", "two"}));
  CHECK(backend->GetString("/MetadataChooser/selected") == "");

  backend->SetString("/MetadataChooser/selected", "one");
  wpi::tunables::TunableRegistry::Update();

  CHECK(backend->GetString("/MetadataChooser/selected") == "one");
  CHECK(chooser.GetSelected() == 1);
}

TEST_CASE("SelectableTest DefaultIsReturnedOnNoSelect",
          "[tunable][selectable]") {
  wpi::tunables::Selectable<int> chooser;

  for (int i = 1; i <= 3; i++) {
    chooser.Add(std::to_string(i), i);
  }

  // Use 4 here rather than 0 to make sure it's not default-init int.
  chooser.AddDefault("4", 4);

  CHECK(chooser.GetSelected() == 4);
}

TEST_CASE_METHOD(SelectableFixture, "SelectableTest DefaultIsReturnedOnUnknown",
                 "[tunable][selectable]") {
  wpi::tunables::Selectable<int> chooser;
  chooser.AddDefault("one", 1);
  chooser.Add("two", 2);

  wpi::tunables::Publish("UnknownDefaultChooser", chooser);
  backend->SetString("/UnknownDefaultChooser/selected", "missing");
  wpi::tunables::TunableRegistry::Update();

  CHECK(chooser.GetSelected() == 1);
}

TEST_CASE("SelectableTest DefaultConstructableIsReturnedOnNoSelectAndNoDefault",
          "[tunable][selectable]") {
  wpi::tunables::Selectable<int> chooser;

  for (int i = 1; i <= 3; i++) {
    chooser.Add(std::to_string(i), i);
  }

  CHECK(chooser.GetSelected() == 0);
}

TEST_CASE_METHOD(SelectableFixture, "SelectableTest ChangeListener",
                 "[tunable][selectable]") {
  wpi::tunables::Selectable<int> chooser;
  for (int i = 1; i <= 3; i++) {
    chooser.Add(std::to_string(i), i);
  }
  int currentVal = 0;
  chooser.OnChange([&](int val) { currentVal = val; });

  wpi::tunables::Publish("ChangeListenerChooser", chooser);
  wpi::tunables::TunableRegistry::Update();
  backend->SetString("/ChangeListenerChooser/selected", "3");
  wpi::tunables::TunableRegistry::Update();

  CHECK(currentVal == 3);
}

TEST_CASE_METHOD(SelectableFixture,
                 "SelectableTest ChangeListenerUsesDefaultWhenSelectionCleared",
                 "[tunable][selectable]") {
  wpi::tunables::Selectable<int> chooser;
  chooser.AddDefault("one", 1);
  chooser.Add("two", 2);
  int currentVal = 0;
  chooser.OnChange([&](int val) { currentVal = val; });

  wpi::tunables::Publish("ChangeListenerDefaultChooser", chooser);
  backend->SetString("/ChangeListenerDefaultChooser/selected", "two");
  wpi::tunables::TunableRegistry::Update();

  CHECK(chooser.GetSelected() == 2);
  CHECK(currentVal == 2);

  backend->SetString("/ChangeListenerDefaultChooser/selected", "");
  wpi::tunables::TunableRegistry::Update();

  CHECK(chooser.GetSelected() == 1);
  CHECK(currentVal == 1);

  currentVal = 0;
  backend->SetString("/ChangeListenerDefaultChooser/selected", "missing");
  wpi::tunables::TunableRegistry::Update();

  CHECK(chooser.GetSelected() == 1);
  CHECK(currentVal == 1);
}

TEST_CASE_METHOD(SelectableFixture,
                 "SelectableTest MoveConstructRebindsChangeListener",
                 "[tunable][selectable]") {
  wpi::tunables::Selectable<int> chooser;
  chooser.Add("one", 1);
  chooser.Add("two", 2);
  int currentVal = 0;
  chooser.OnChange([&](int val) { currentVal = val; });

  wpi::tunables::Selectable<int> movedChooser{std::move(chooser)};
  wpi::tunables::Publish("MoveConstructChooser", movedChooser);
  backend->SetString("/MoveConstructChooser/selected", "two");
  wpi::tunables::TunableRegistry::Update();

  CHECK(movedChooser.GetSelected() == 2);
  CHECK(currentVal == 2);
}

TEST_CASE_METHOD(
    SelectableFixture,
    "SelectableTest PublishedMoveAssignTransfersEntriesAndChangeListener",
    "[tunable][selectable]") {
  wpi::tunables::Selectable<int> source;
  source.Add("one", 1);
  int currentVal = 0;
  source.OnChange([&](int val) { currentVal = val; });

  wpi::tunables::Selectable<int> destination;
  destination.Add("old", 2);

  wpi::tunables::Publish("MoveAssignSourceChooser", source);
  wpi::tunables::Publish("MoveAssignDestinationChooser", destination);
  auto sourceSelectedUid = backend->GetUid("/MoveAssignSourceChooser/selected");
  auto destinationSelectedUid =
      backend->GetUid("/MoveAssignDestinationChooser/selected");
  REQUIRE(sourceSelectedUid);
  REQUIRE(destinationSelectedUid);

  destination = std::move(source);

  CHECK(backend->GetUid("/MoveAssignSourceChooser/selected") ==
        sourceSelectedUid);
  CHECK_FALSE(backend->GetUid("/MoveAssignDestinationChooser/selected"));
  CHECK_FALSE(
      wpi::tunables::TunableRegistry::GetTunable(*destinationSelectedUid));

  backend->SetString("/MoveAssignSourceChooser/selected", "one");
  wpi::tunables::TunableRegistry::Update();

  CHECK(destination.GetSelected() == 1);
  CHECK(currentVal == 1);
}

TEST_CASE_METHOD(SelectableFixture,
                 "SelectableTest "
                 "PublishedMoveAssignToUnpublishedDestinationTransfersEntries",
                 "[tunable][selectable]") {
  wpi::tunables::Selectable<int> destination;
  int currentVal = 0;

  {
    wpi::tunables::Selectable<int> source;
    source.Add("one", 1);
    source.OnChange([&](int val) { currentVal = val; });

    wpi::tunables::Publish("MoveAssignUnpublishedSourceChooser", source);
    REQUIRE(backend->GetUid("/MoveAssignUnpublishedSourceChooser/selected"));

    destination = std::move(source);
  }

  REQUIRE(backend->GetUid("/MoveAssignUnpublishedSourceChooser/selected"));

  backend->SetString("/MoveAssignUnpublishedSourceChooser/selected", "one");
  wpi::tunables::TunableRegistry::Update();

  CHECK(destination.GetSelected() == 1);
  CHECK(currentVal == 1);
}

TEST_CASE_METHOD(SelectableFixture,
                 "SelectableTest ListenerIsNotCalledForUnknownSelection",
                 "[tunable][selectable]") {
  wpi::tunables::Selectable<int> chooser;
  chooser.Add("one", 1);
  int currentVal = 0;
  chooser.OnChange([&](int val) { currentVal = val; });

  wpi::tunables::Publish("UnknownSelectionChooser", chooser);
  backend->SetString("/UnknownSelectionChooser/selected", "missing");
  wpi::tunables::TunableRegistry::Update();

  CHECK(currentVal == 0);
  CHECK(chooser.GetSelected() == 0);
}

TEST_CASE_METHOD(SelectableFixture,
                 "SelectableTest ListenerReplacementUsesLatestListener",
                 "[tunable][selectable]") {
  wpi::tunables::Selectable<int> chooser;
  chooser.Add("one", 1);
  int first = 0;
  int second = 0;
  chooser.OnChange([&](int val) { first = val; });
  chooser.OnChange([&](int val) { second = val; });

  wpi::tunables::Publish("ListenerReplacementChooser", chooser);
  backend->SetString("/ListenerReplacementChooser/selected", "one");
  wpi::tunables::TunableRegistry::Update();

  CHECK(first == 0);
  CHECK(second == 1);
}

TEST_CASE_METHOD(
    SelectableFixture,
    "SelectableTest DuplicateOptionReplacesValueWithoutDuplicatingOptions",
    "[tunable][selectable]") {
  wpi::tunables::Selectable<int> chooser;
  chooser.Add("mode", 1);
  chooser.Add("mode", 2);

  wpi::tunables::Publish("DuplicateChooser", chooser);
  backend->SetString("/DuplicateChooser/selected", "mode");
  wpi::tunables::TunableRegistry::Update();

  CHECK(chooser.GetSelected() == 2);
  CHECK(backend->GetStringVector("/DuplicateChooser/options") ==
        (std::vector<std::string>{"mode"}));
}

TEST_CASE("SelectableTest RemoveClearsDefaultWhenRemoved",
          "[tunable][selectable]") {
  wpi::tunables::Selectable<int> chooser;
  chooser.AddDefault("one", 1);

  chooser.Remove("one");
  chooser.Add("one", 11);

  CHECK(chooser.GetSelected() == 0);
}

TEST_CASE_METHOD(SelectableFixture,
                 "SelectableTest RemovePreservesSelectedNameUntilReadded",
                 "[tunable][selectable]") {
  wpi::tunables::Selectable<int> chooser;
  chooser.Add("one", 1);
  chooser.Add("two", 2);
  wpi::tunables::Publish("RemoveChooser", chooser);
  backend->SetString("/RemoveChooser/selected", "two");
  wpi::tunables::TunableRegistry::Update();

  chooser.Remove("two");
  CHECK(chooser.GetSelected() == 0);
  CHECK(backend->GetStringVector("/RemoveChooser/options") ==
        (std::vector<std::string>{"one"}));

  chooser.Add("two", 22);
  CHECK(chooser.GetSelected() == 22);
  CHECK(backend->GetStringVector("/RemoveChooser/options") ==
        (std::vector<std::string>{"one", "two"}));

  chooser.Remove("missing");
  CHECK(chooser.GetSelected() == 22);
  CHECK(backend->GetStringVector("/RemoveChooser/options") ==
        (std::vector<std::string>{"one", "two"}));
}

TEST_CASE_METHOD(
    SelectableFixture,
    "SelectableTest ClearResetsOptionsAndDefaultButPreservesSelectionName",
    "[tunable][selectable]") {
  wpi::tunables::Selectable<int> chooser;
  chooser.AddDefault("one", 1);
  chooser.Add("two", 2);
  wpi::tunables::Publish("ClearChooser", chooser);
  backend->SetString("/ClearChooser/selected", "two");
  wpi::tunables::TunableRegistry::Update();

  chooser.Clear();
  CHECK(chooser.GetSelected() == 0);
  CHECK(backend->GetString("/ClearChooser/default") == "");
  CHECK(backend->GetStringVector("/ClearChooser/options").empty());

  chooser.Add("two", 22);
  CHECK(chooser.GetSelected() == 22);
  CHECK(backend->GetStringVector("/ClearChooser/options") ==
        (std::vector<std::string>{"two"}));
}

TEST_CASE("SelectableTest SharedPtrSelectionReturnsWeakPtr",
          "[tunable][selectable]") {
  auto selected = std::make_shared<int>(5);
  wpi::tunables::Selectable<std::shared_ptr<int>> chooser;
  chooser.Add("selected", selected);

  auto weakSelected = chooser.GetSelected();
  CHECK(weakSelected.expired());

  chooser.AddDefault("default", std::make_shared<int>(6));
  CHECK(*chooser.GetSelected().lock() == 6);
}
